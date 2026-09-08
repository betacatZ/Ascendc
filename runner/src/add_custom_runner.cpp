// AddCustom runner
//
// 通过 HiAI 加载 OMC 模型完成 AddCustom 算子推理，
// 输入 x/y 为 fp16 格式，输出 z 为 fp16 格式，
// 把输入和输出以二进制 fp16 格式 dump 到指定目录。

#include "HiAiModelManagerService.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Fp16Bits = std::uint16_t;

struct Options {
    std::string model_path = "/data/local/tmp/single-op-kirin/add-custom/custom_graph.omc";
    std::string model_name = "AddCustom";
    std::string dump_dir = "/data/local/tmp/single-op-kirin/add-custom";
    std::size_t batches = 1;
    std::uint32_t seed_x = 0x12345678U;
    std::uint32_t seed_y = 0x9abcdef0U;
    int bias = 0;
};

Fp16Bits float_to_fp16(float value)
{
    __fp16 half = static_cast<__fp16>(value);
    Fp16Bits bits;
    std::memcpy(&bits, &half, sizeof(bits));
    return bits;
}

float fp16_to_float(Fp16Bits value)
{
    __fp16 half;
    std::memcpy(&half, &value, sizeof(half));
    return static_cast<float>(half);
}

void check_ai(hiai::AIStatus status, const char *label)
{
    if (status != hiai::AI_SUCCESS) {
        throw std::runtime_error(std::string(label) + " failed: " + std::to_string(status));
    }
}

std::size_t parse_size(const char *raw, const char *name)
{
    if (raw == nullptr || raw[0] == '\0') {
        throw std::invalid_argument(std::string("missing value for ") + name);
    }
    char *end = nullptr;
    const unsigned long long parsed = std::strtoull(raw, &end, 10);
    if (end == raw || *end != '\0' || parsed == 0 ||
        parsed > static_cast<unsigned long long>(std::numeric_limits<std::size_t>::max())) {
        throw std::invalid_argument(std::string("invalid value for ") + name + ": " + raw);
    }
    return static_cast<std::size_t>(parsed);
}

int parse_int(const char *raw, const char *name)
{
    if (raw == nullptr || raw[0] == '\0') {
        throw std::invalid_argument(std::string("missing value for ") + name);
    }
    char *end = nullptr;
    const long parsed = std::strtol(raw, &end, 10);
    if (end == raw || *end != '\0' ||
        parsed < std::numeric_limits<int>::min() ||
        parsed > std::numeric_limits<int>::max()) {
        throw std::invalid_argument(std::string("invalid value for ") + name + ": " + raw);
    }
    return static_cast<int>(parsed);
}

std::uint32_t parse_u32(const char *raw, const char *name)
{
    if (raw == nullptr || raw[0] == '\0') {
        throw std::invalid_argument(std::string("missing value for ") + name);
    }
    char *end = nullptr;
    const unsigned long parsed = std::strtoul(raw, &end, 10);
    if (end == raw || *end != '\0' || parsed == 0 ||
        parsed > std::numeric_limits<std::uint32_t>::max()) {
        throw std::invalid_argument(std::string("invalid value for ") + name + ": " + raw);
    }
    return static_cast<std::uint32_t>(parsed);
}

void print_usage(const char *program)
{
    std::cout
        << "Usage: " << program << " [options]\n"
        << "  --model PATH        OMC file (default /data/local/tmp/single-op-kirin/add-custom/custom_graph.omc)\n"
        << "  --model-name NAME   HIAI model name (default AddCustom)\n"
        << "  --dump-dir DIR      directory for x/y/z dumps (default /data/local/tmp/single-op-kirin/add-custom)\n"
        << "  --batches N         repeated inference rounds (default 1)\n"
        << "  --seed-x N          RNG seed for input x (default 0x12345678)\n"
        << "  --seed-y N          RNG seed for input y (default 0x9abcdef0)\n"
        << "  --bias N            bias parameter for AddCustom operator (default 0)\n"
        << "  --help              show this help\n";
}

Options parse_options(int argc, char **argv)
{
    Options options;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        const auto next_value = [&]() -> const char * {
            if (index + 1 >= argc) {
                throw std::invalid_argument("missing value after " + argument);
            }
            return argv[++index];
        };

        if (argument == "--model") {
            options.model_path = next_value();
        } else if (argument == "--model-name") {
            options.model_name = next_value();
        } else if (argument == "--dump-dir") {
            options.dump_dir = next_value();
        } else if (argument == "--batches") {
            options.batches = parse_size(next_value(), "--batches");
        } else if (argument == "--seed-x") {
            options.seed_x = parse_u32(next_value(), "--seed-x");
        } else if (argument == "--seed-y") {
            options.seed_y = parse_u32(next_value(), "--seed-y");
        } else if (argument == "--bias") {
            options.bias = parse_int(next_value(), "--bias");
        } else if (argument == "--help" || argument == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else {
            throw std::invalid_argument("unknown argument: " + argument);
        }
    }
    return options;
}

std::vector<std::uint8_t> read_binary_file(const std::string &path)
{
    std::ifstream stream(path, std::ios::binary | std::ios::ate);
    if (!stream) {
        throw std::runtime_error("failed to open model: " + path);
    }
    const auto end = stream.tellg();
    if (end <= 0 || static_cast<std::uint64_t>(end) > std::numeric_limits<std::uint32_t>::max()) {
        throw std::runtime_error("invalid model file size: " + path);
    }
    std::vector<std::uint8_t> data(static_cast<std::size_t>(end));
    stream.seekg(0, std::ios::beg);
    stream.read(reinterpret_cast<char *>(data.data()), static_cast<std::streamsize>(data.size()));
    if (!stream) {
        throw std::runtime_error("failed to read model: " + path);
    }
    return data;
}

std::size_t element_count(const hiai::TensorDimension &dimension)
{
    std::size_t result = 1;
    for (const auto value : dimension.GetDims()) {
        if (value == 0 || result > std::numeric_limits<std::size_t>::max() / value) {
            throw std::runtime_error("invalid HIAI tensor dimension");
        }
        result *= value;
    }
    return result;
}

std::string dims_to_string(const std::vector<hiai::TensorDimension> &dimensions)
{
    std::string result = "[";
    for (std::size_t index = 0; index < dimensions.size(); ++index) {
        if (index != 0) {
            result += ", ";
        }
        result += "{";
        const auto values = dimensions[index].GetDims();
        for (std::size_t dim = 0; dim < values.size(); ++dim) {
            if (dim != 0) {
                result += "x";
            }
            result += std::to_string(values[dim]);
        }
        result += "}";
    }
    result += "]";
    return result;
}

std::vector<Fp16Bits> make_input(std::size_t count, std::uint32_t seed)
{
    std::vector<Fp16Bits> values(count);
    std::uint32_t state = seed;
    for (auto &value : values) {
        state = state * 1664525U + 1013904223U;
        const auto centered = static_cast<std::int32_t>((state >> 16U) & 0xffffU) - 32768;
        value = float_to_fp16(static_cast<float>(centered) / 262144.0F);
    }
    return values;
}

void copy_to_tensor(hiai::AiTensor &tensor,
                    const std::vector<Fp16Bits> &source,
                    const char *label)
{
    const std::size_t bytes = source.size() * sizeof(source[0]);
    if (tensor.GetBuffer() == nullptr || tensor.GetSize() < bytes) {
        throw std::runtime_error(std::string(label) + " HIAI tensor buffer is too small");
    }
    std::memcpy(tensor.GetBuffer(), source.data(), bytes);
}

void dump_fp16(const std::string &path,
               const Fp16Bits *data,
               std::size_t count)
{
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    if (!stream) {
        throw std::runtime_error("failed to open dump file: " + path);
    }
    stream.write(reinterpret_cast<const char *>(data),
                 static_cast<std::streamsize>(count * sizeof(Fp16Bits)));
    if (!stream) {
        throw std::runtime_error("failed to write dump file: " + path);
    }
}

class LoadedModelGuard {
public:
    explicit LoadedModelGuard(hiai::AiModelMngerClient &client) : client_(&client) {}
    ~LoadedModelGuard()
    {
        if (client_ != nullptr) {
            const auto status = client_->UnLoadModel();
            if (status != hiai::AI_SUCCESS) {
                std::cerr << "warning: HIAI UnLoadModel failed: " << status << '\n';
            }
        }
    }

    LoadedModelGuard(const LoadedModelGuard &) = delete;
    LoadedModelGuard &operator=(const LoadedModelGuard &) = delete;

private:
    hiai::AiModelMngerClient *client_;
};

void load_model(hiai::AiModelMngerClient &client,
                const Options &options,
                const std::vector<std::uint8_t> &model_data)
{
    auto description = std::make_shared<hiai::AiModelDescription>(
        options.model_name,
        hiai::AiModelDescription_Frequency_HIGH,
        hiai::HIAI_FRAMEWORK_NONE,
        hiai::HIAI_MODELTYPE_OFFLINE,
        hiai::AiModelDescription_DeviceType_NPU);
    check_ai(description->SetModelBuffer(model_data.data(),
                                         static_cast<std::uint32_t>(model_data.size())),
             "HIAI SetModelBuffer");

    bool compatible = false;
    check_ai(client.CheckModelCompatibility(*description, compatible),
             "HIAI CheckModelCompatibility");
    std::cout << "HIAI compatibility: " << (compatible ? "yes" : "no") << '\n';
    if (!compatible) {
        throw std::runtime_error("the OMC model is not compatible with this device");
    }

    std::vector<std::shared_ptr<hiai::AiModelDescription>> models{description};
    check_ai(client.Load(models), "HIAI Load");
}

int run(const Options &options,
        hiai::AiModelMngerClient &client,
        const std::vector<hiai::TensorDimension> &input_dimensions,
        const std::vector<hiai::TensorDimension> &output_dimensions,
        const std::vector<Fp16Bits> &x,
        const std::vector<Fp16Bits> &y)
{
    auto x_tensor = std::make_shared<hiai::AiTensor>();
    auto y_tensor = std::make_shared<hiai::AiTensor>();
    auto z_tensor = std::make_shared<hiai::AiTensor>();
    check_ai(x_tensor->Init(&input_dimensions[0], hiai::HIAI_DATATYPE_FLOAT16), "HIAI x tensor");
    check_ai(y_tensor->Init(&input_dimensions[1], hiai::HIAI_DATATYPE_FLOAT16), "HIAI y tensor");
    check_ai(z_tensor->Init(&output_dimensions[0], hiai::HIAI_DATATYPE_FLOAT16), "HIAI z tensor");
    copy_to_tensor(*x_tensor, x, "x");
    copy_to_tensor(*y_tensor, y, "y");

    std::vector<std::shared_ptr<hiai::AiTensor>> inputs{x_tensor, y_tensor};
    std::vector<std::shared_ptr<hiai::AiTensor>> outputs{z_tensor};
    hiai::AiContext context;
    context.SetPara("model_name", options.model_name);
    int32_t stamp = 0;

    check_ai(client.Process(context, inputs, outputs, 30000, stamp), "HIAI Process");

    const std::size_t output_count = element_count(output_dimensions[0]);
    const auto *npu_out = static_cast<const Fp16Bits *>(z_tensor->GetBuffer());
    if (npu_out == nullptr || z_tensor->GetSize() < output_count * sizeof(Fp16Bits)) {
        throw std::runtime_error("HIAI output tensor buffer is invalid");
    }

    const std::string dir = options.dump_dir;
    dump_fp16(dir + "/add_custom_x.bin", x.data(), x.size());
    dump_fp16(dir + "/add_custom_y.bin", y.data(), y.size());
    dump_fp16(dir + "/add_custom_z.bin", npu_out, output_count);

    std::cout << std::fixed << std::setprecision(4)
              << "AddCustom inference complete: bias=" << options.bias
              << " dumped x=" << x.size() << " y=" << y.size() << " z=" << output_count << '\n'
              << "dump files: " << dir << "/add_custom_x.bin "
              << dir << "/add_custom_y.bin " << dir << "/add_custom_z.bin\n";
    return 0;
}

} // namespace

int main(int argc, char **argv)
{
    try {
        const Options options = parse_options(argc, argv);
        const auto model_data = read_binary_file(options.model_path);

        hiai::AiModelMngerClient client;
        check_ai(client.Init(nullptr), "HIAI Init");

        load_model(client, options, model_data);
        LoadedModelGuard unload_guard(client);

        std::vector<hiai::TensorDimension> input_dimensions;
        std::vector<hiai::TensorDimension> output_dimensions;
        check_ai(client.GetModelIOTensorDim(options.model_name, input_dimensions,
                                            output_dimensions),
                 "HIAI GetModelIOTensorDim");
        std::cout << "HIAI model dims: inputs=" << dims_to_string(input_dimensions)
                  << " outputs=" << dims_to_string(output_dimensions) << '\n';
        if (input_dimensions.size() != 2 || output_dimensions.size() != 1) {
            throw std::runtime_error("model must have two inputs and one output");
        }

        const std::size_t x_count = element_count(input_dimensions[0]);
        const std::size_t y_count = element_count(input_dimensions[1]);
        const std::size_t z_count = element_count(output_dimensions[0]);

        std::cout << "AddCustom shape: x=" << x_count << " y=" << y_count
                  << " z=" << z_count << " batches=" << options.batches
                  << " bias=" << options.bias << '\n';

        const auto x = make_input(x_count, options.seed_x);
        const auto y = make_input(y_count, options.seed_y);

        for (std::size_t run_index = 0; run_index < options.batches; ++run_index) {
            const auto start = std::chrono::steady_clock::now();
            const int result = run(options, client, input_dimensions, output_dimensions, x, y);
            const double elapsed_ms =
                std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start)
                    .count();
            std::cout << "batch[" << run_index << "]: wall_ms=" << std::fixed
                      << std::setprecision(3) << elapsed_ms << '\n';
            if (result != 0) {
                return result;
            }
        }
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "add-custom runner failed: " << error.what() << '\n';
        return 2;
    }
}
