#pragma once
#include <cstdint>
#include <expected>
#include <string>
namespace TheosRenderPipeline::NeuralRendering {
enum class ErrorKind { InvalidInput, Unsupported, IdentityMismatch, Conflict, Io, Runtime, Retirement };
struct Error { ErrorKind kind; int64_t nativeCode{}; std::string message; };
template<class T> using Result = std::expected<T, Error>;
}
