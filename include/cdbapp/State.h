/*
 * State.h
 *
 *  Created on: Jan 18, 2012
 *     Authors: John Mangan <jmangan@eng.ucsd.edu>,
 *              David Srour <dsrour@eng.ucsd.edu>
 */

#ifndef STATE_H_
#define STATE_H_

#include <string>
#include <map>
#include <list>
#include <vector>

#include <json/json.h>
#include <cdbapp/UuidGenerator.h>

class State
{
public:
    typedef Json::Value VARIABLES;

    enum ErrorType
    {
        NO_ERROR = 0,
        DOES_NOT_EXIST,
        INVALID_TYPE
    };

    State(VARIABLES variables = sEmptyVariables, std::string const uuid = UuidGenerator::GenerateUuid());

    void
    SetVariable(std::string const& variableName, bool const value);

    void
    SetVariable(std::string const& variableName, int const value);

    void
    SetVariable(std::string const& variableName, double const value);

    void
    SetVariable(std::string const& variableName, std::string const& value);

    void
    SetVariable(std::string const& variableName, std::vector< double > const& values);

    void
    SetVariable(std::string const& variableName, std::vector< std::string > const& values);

    ErrorType
    GetVariable(std::string const& variableName, bool& value);

    ErrorType
    GetVariable(std::string const& variableName, int& value);

    ErrorType
    GetVariable(std::string const& variableName, double& value);

    ErrorType
    GetVariable(std::string const& variableName, std::string& value);

    ErrorType
    GetVariable(std::string const& variableName, std::vector< double >& values);

    ErrorType
    GetVariable(std::string const& variableName, std::vector< std::string >& values);

    VARIABLES const&
    Variables(void) const
    { return mVariables; }

    void
    Variables(VARIABLES const& variables)
    { mVariables = variables; }

    static void
    ConvertJsonStateArrayToStrings( std::string const& jsonStateArray, std::list< std::string >& strings );

    static void
    ConvertStringsToJsonStateArray( std::list< std::string >& strings, std::string& jsonStateArray );

    static State 
    Read(std::string const& json);    

    std::string
    Write(bool formatted = false);

    std::string const&
    Uuid(void) const
    { return mUuid; }

private:
    std::string         mUuid;
    VARIABLES           mVariables;
    static VARIABLES    sEmptyVariables;
};

#endif /* STATE_H_ */
