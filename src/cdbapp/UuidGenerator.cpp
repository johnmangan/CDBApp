/*
 * UuidGenerator.cpp
 *
 *  Created on: Apr 16, 2012
 *     Authors: John Mangan <jmangan@eng.ucsd.edu>,
 *              David Srour <dsrour@eng.ucsd.edu>
 */


#include <uuid/uuid.h>
#include <cdbapp/UuidGenerator.h>

namespace cdb {

namespace UuidGenerator {

    std::string GenerateUuid(void)
    {
        uuid_t uuid;
        uuid_generate(uuid);
        char str_id[36];
        uuid_unparse_lower(uuid, str_id);

        return std::string(str_id);
    }

}

}

