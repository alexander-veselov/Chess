#pragma once

#include "chess/application/application.h"

#include <memory>

namespace chess {

std::shared_ptr<Application> CreateApplication();

}; // namespace chess