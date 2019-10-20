#ifndef TRACER_H_
#define TRACER_H_

#include <string>
#include <vector>

namespace GravelBox {

/**
 * Tracer class spawns and traces child processes.
 */
class Tracer {
  public:
	/**
	 * Spawn and trace a child process.
	 * Return after the child process exits.
	 *
	 * @param args the arguments used to spawn processes.
	 */
	void run(const std::vector<std::string> &args) const;
};

}  // namespace GravelBox

#endif  // TRACER_H_
