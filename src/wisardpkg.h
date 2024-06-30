//Base
#include "base.h"

//Json
#include "libs/json.hpp"
namespace nl = nlohmann;

//Common
#include "common/definetypes.cc"
#include "common/exceptions.cc"
#include "common/utils.cc"

//Data
#include "data/bininput.cc"
#include "data/dataset.cc"

//Model
#include "models/base/model.cc"
#include "models/base/regressionmodel.cc"

//RegressionWisard
#include "models/regressionwisard/regressionramdatahandle.cc"
#include "models/regressionwisard/regressionram.cc"
#include "models/regressionwisard/regressionwisard.cc"
