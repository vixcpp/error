# Vix Error

The **error module** provides a unified, explicit, and lightweight system
for handling failures in Vix.

It is designed as a foundational building block for:
- filesystem operations
- networking and transport layers
- parsing and validation
- CLI tools
- backend services

The module favors **explicit control flow**, **predictability**, and **zero hidden behavior**.

## Philosophy

Error handling in Vix follows strict rules:

- No hidden exceptions
- Errors are explicit values
- No implicit control flow
- Minimal overhead
- Stable and composable APIs

Every operation returns either:
- a value
- or a structured `Error`

## Core Concepts

| Concept | Purpose |
|--------|--------|
| `ErrorCode` | What went wrong |
| `ErrorCategory` | Where the error happened |
| `Error` | Structured error object |
| `Result<T>` | Value or error |
| `Exception` | Optional exception wrapper |

## Error Model

A failure is represented by:

```cpp
Error
```

Which contains:

- code — semantic error code
- category — domain (io, network, validation…)
- message — human-readable description

Example:

```cpp
Error err(
  ErrorCode::InvalidArgument,
  ErrorCategory::validation(),
  "invalid port"
);
```

## Using Result<T>

`Result<T>` is the primary abstraction for returning values.

```cpp
#include <vix/error/Result.hpp>

Result<int> divide(int a, int b)
{
  if (b == 0)
  {
    return Error(
      ErrorCode::InvalidArgument,
      ErrorCategory::validation(),
      "division by zero"
    );
  }

  return a / b;
}
```

### Usage

```cpp
auto r = divide(10, 0);

if (!r)
{
  std::cout << r.error().message() << "\n";
}
else
{
  std::cout << r.value() << "\n";
}
```

## Chaining Operations

Results can be composed without exceptions:

```cpp
auto result =
  parse_int("8080")
    .and_then(validate_port)
    .map([](int port) {
      return port * 2;
    });
```

This allows building pipelines with clear error propagation.

## Result<void>

Operations without a return value use:

```cpp
Result<void> write_file(...)
{
  if (fail)
    return Error(...);

  return {};
}
```

## Error Categories

Categories group errors by domain:

- `ErrorCategory::io()`
- `ErrorCategory::network()`
- `ErrorCategory::validation()`
- `ErrorCategory::system()`
- `ErrorCategory::internal()`

They are lightweight and allocation-free.

## Exception Interoperability

Vix does not require exceptions, but supports them when needed.

```cpp
throw Exception(
  ErrorCode::IoError,
  ErrorCategory::io(),
  "failed to open file"
);
```

Catch:

```cpp
catch (const Exception& e)
{
  std::cout << e.what() << "\n";
}
```

## Examples

See `examples/`:

- basic.cpp
- result.cpp
- chaining.cpp
- exception.cpp

Run:

```bash
vix run examples/result.cpp
```

## Performance Notes

- No hidden allocations (except error message)
- `Result<T>` uses `std::variant`
- Categories use `string_view` (no allocation)
- No exceptions in normal flow

Suitable for:

- hot paths
- low-latency systems
- embedded environments

## Relationship with Other Modules

The error module is used everywhere in Vix:

- `fs` → file operations return `Result<T>`
- `conversion` → returns structured errors
- `validation` → builds on explicit error propagation
- `http / p2p` → network failures modeled as values

It is the foundation of error handling in Vix.

## Design Goals Recap

- Errors as values
- Explicit control flow
- Composable APIs
- No surprises
- Minimal runtime cost

## License

MIT
Part of the Vix.cpp project.

