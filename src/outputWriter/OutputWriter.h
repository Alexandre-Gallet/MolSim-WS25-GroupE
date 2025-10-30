//
// Created by darig on 10/29/2025.
//

#ifndef MOLSIM_GROUPE_OUTPUTWRITER_H
#define MOLSIM_GROUPE_OUTPUTWRITER_H

namespace outputWriter {

  class OutputWriter {
    public:
      virtual ~OutputWriter() = default;

    //void plotParticles(std::list<Particle> particles, const std::string &filename, int iteration);
  };
}


#endif  // MOLSIM_GROUPE_OUTPUTWRITER_H
