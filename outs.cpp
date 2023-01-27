#include "outs.h"

Verbosity verbosity = Verbosity::Diagnostic;

Verbosity getVerbosity() {
    return verbosity;
}

void setVerbosity(Verbosity verbo){
    verbosity = verbo;
}
