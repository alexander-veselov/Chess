#pragma once

#include "application.h"

#include <memory>

namespace chess {

std::shared_ptr<Application> CreateApplication();

};  // namespace chess