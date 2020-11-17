/*
 * Copyright 2020 Javier Jorge. All rights reserved.
 * License: https://github.com/JJorgeDSIC/CppDecoder#license
 */

inline std::vector<Frame>::iterator Sample::begin() noexcept {
  return frames.begin();
}

inline std::vector<Frame>::iterator Sample::end() noexcept {
  return frames.end();
}

inline std::vector<Frame>::const_iterator Sample::cbegin() const noexcept {
  return frames.cbegin();
}
inline std::vector<Frame>::const_iterator Sample::cend() const noexcept {
  return frames.cend();
}