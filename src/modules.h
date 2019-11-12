#ifndef MODULES_H_
#define MODULES_H_

#include <boost/program_options.hpp>

namespace GravelBox {

/**
 * Run GravelBox with some type of Tracer/Parser/Config/UI/Logger.
 * By separating the module instantiations from the main program helps prevent
 * recompiling main.o every time when we switch modules.
 *
 * @param vm program options.
 * @return int program return value.
 */
int run(const boost::program_options::variables_map &vm);

}  // namespace GravelBox

#endif  // MODULES_H_
