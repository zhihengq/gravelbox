#ifndef TRACER_H_
#define TRACER_H_

#include <string>
#include <vector>

namespace GravelBox {

class Tracer {
 public:
	void run(const std::vector<std::string> &target) const;
};

}  // namespace GravelBox

#endif  // TRACER_H_
