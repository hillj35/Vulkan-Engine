#pragma once

#include "../lve_swap_chain.hpp"
#include "../lve_types.hpp"
#include "../pipeline_builder.hpp"

namespace init {
void createPipelines(VkDevice device, lve::LveSwapChain *swapChain,
                     lve::ApplicationPipelines *outPipelines);
void createComputePipelines(VkDevice device, lve::LveSwapChain *swapChain,
                            lve::ApplicationPipelines *outPipelines);
} // namespace init