#ifndef CHRONO_EXTI_DEF_H_
#define CHRONO_EXTI_DEF_H_

#include "stm32_def.h"

#undef CHRONO_EXTI_IRQn
#undef CHRONO_IRQHandler

#if defined(STM32F042x6)
# define CHRONO_EXTI_IRQn   PVD_VDDIO2_IRQn
# define CHRONO_IRQHandler  PVD_VDDIO2_IRQHandler
#elif  defined(STM32G0xx) || defined(STM32F4xx) || defined(STM32L0xx) || defined(STM32L1xx) || defined(STM32F0xx)
# define CHRONO_EXTI_IRQn   PVD_IRQn
# define CHRONO_IRQHandler  PVD_IRQHandler

#elif defined(STM32WBxx) || defined(STM32L4xx) || defined(STM32G4xx)
# define CHRONO_EXTI_IRQn   PVD_PVM_IRQn
# define CHRONO_IRQHandler  PVD_PVM_IRQHandler

#else
# error "cannot resolve chrono IRQ"
#endif



#endif
