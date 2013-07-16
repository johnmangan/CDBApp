/*
 * State.cpp
 *
 *  Created on: Jan 21, 2012
 *     Authors: John Mangan <jmangan@eng.ucsd.edu>,
 *              David Srour <dsrour@eng.ucsd.edu>
 */

#include <sstream>
#include <iostream>
#include <cdbapp/State.h>
#include <json/json.h>

#define STREAM_PRECISION 15

/*static*/ State::VARIABLES State::sEmptyVariables;

State::State(VARIABLES variables /*= sEmptyVariables*/, std::string const uuid /*= UuidGenerator::GenerateUuid()*/)
    : mUuid(uuid), mVariables(variables)
{
}

void
State::SetVariable(std::string const& variableName, bool const value)
{
    mVariables[variableName] = value;
}

void
State::SetVariable(std::string const& variableName, int const value)
{
    mVariables[variableName] = value;
}

void
State::SetVariable(std::string const& variableName, double const value)
{
    mVariables[variableName] = value;
}

void
State::SetVariable(std::string const& variableName, std::string const& value)
{
    mVariables[variableName] = value;
}

void
State::SetVariable(std::string const& variableName, std::vector< double > const& values)
{
    Json::Value array;
 
    for (unsigned int i = 0; i < values.size(); i++)
        array.append(values[i]);

    mVariables[variableName] = array;
}

void
State::SetVariable(std::string const& variableName, std::vector< std::string > const& values)
{
    Json::Value array;
 
    for (unsigned int i = 0; i < values.size(); i++)
        array.append(values[i]);

    mVariables[variableName] = array;
}

State::ErrorType
State::GetVariable(std::string const& variableName, bool& value)
{
    if (!mVariables.isMember(variableName))
        return DOES_NOT_EXIST; 

    Json::Value v = mVariables[variableName];

    if (!v.isBool())
        return INVALID_TYPE;

    value = v.asBool();
    return NO_ERROR;
}

State::ErrorType
State::GetVariable(std::string const& variableName, int& value)
{
    if (!mVariables.isMember(variableName))
        return DOES_NOT_EXIST;

    Json::Value v = mVariables[variableName];

    if (!v.isNumeric())
        return INVALID_TYPE;

    value = v.asInt();
    return NO_ERROR;
}

State::ErrorType
State::GetVariable(std::string const& variableName, double& value)
{
    if (!mVariables.isMember(variableName))
        return DOES_NOT_EXIST;

    Json::Value v = mVariables[variableName];

    if (!v.isNumeric())
        return INVALID_TYPE;

    value = v.asDouble();
    return NO_ERROR;
}

State::ErrorType
State::GetVariable(std::string const& variableName, std::string& value)
{
    if (!mVariables.isMember(variableName))
        return DOES_NOT_EXIST;

    Json::Value v = mVariables[variableName];

    if (!v.isString())
        return INVALID_TYPE;

    value = v.asString();
    return NO_ERROR;
}

State::ErrorType
State::GetVariable(std::string const& variableName, std::vector< double >& values)
{
    if (!mVariables.isMember(variableName))
        return DOES_NOT_EXIST;

    Json::Value v = mVariables[variableName];

    if (!v.isArray())
        return INVALID_TYPE;

    values.resize( v.size() );

    for (unsigned int i = 0; i < v.size(); i++)
    {
        Json::Value value = v[i];

        if ( !value.isNumeric() )
            return INVALID_TYPE;

        values[i] = value.asDouble();
    }
    
    return NO_ERROR;
}

State::ErrorType
State::GetVariable(std::string const& variableName, std::vector< std::string >& values)
{
    if (!mVariables.isMember(variableName))
        return DOES_NOT_EXIST;

    Json::Value v = mVariables[variableName];

    if (!v.isArray())
        return INVALID_TYPE;

    values.resize( v.size() );

    for (unsigned int i = 0; i < v.size(); i++)
    {
        Json::Value value = v[i];

        if ( !value.isString() )
            return INVALID_TYPE;

        values[i] = value.asString();
    }
    
    return NO_ERROR;
}

/*static*/ void
State::ConvertJsonStateArrayToStrings( std::string const& jsonStateArray, std::list< std::string >& strings )
{
    Json::Reader reader;
    Json::Value array;

    if (!reader.parse(jsonStateArray, array))
    {
        std::cerr << "Warning: State::ConvertJsonStateArrayToStrings passed an invalid Json string. (Returning unpopulated vector<string>)" << std::endl;
        return;
    }

    if (!array.isArray())
    {
        std::cerr << "Warning: State::ConvertJsonStateArrayToStrings passed a Json string which is not an array. (Returning unpopulated vector<string>)" << std::endl;
        return;
    }

    Json::FastWriter writer;

    for (unsigned int i = 0; i < array.size(); ++i)
        strings.push_back( writer.write( array[i] ) );
}

/*static*/ void
State::ConvertStringsToJsonStateArray( std::list< std::string >& strings, std::string& jsonStateArray )
{
    Json::Reader reader;
    Json::Value array;

    array.resize( strings.size() );

    std::list< std::string >::iterator it;
    unsigned int i = 0;
    for (it = strings.begin(); it != strings.end(); ++it)
    {
        Json::Value value;

        if (!reader.parse(*it, value))
        {
            std::cerr << "Warning: State::ConvertStringsToJsonStateArray passed an invalid Json string. (Skipping string)" << std::endl;
            continue;
        }

        array[i++] = value;
    }

    Json::FastWriter writer;
    jsonStateArray = writer.write( array );
}

/*static*/ State
State::Read(std::string const& json)
{
    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(json, root))
    {
        std::cerr << "Warning: State::Read passed an invalid Json string. (Returning empty state)" << std::endl;
        return State();
    }

    if (root.size() != 1)
    {
        std::cerr << "Warning: State::Read has a State with multiple Uuids. (Returning empty state)" << std::endl;
        return State();
    }

    Json::Value::Members uuids = root.getMemberNames();

    return State(root[uuids[0]], uuids[0]);
}

std::string
State::Write(bool formatted /*= false*/)
{
    Json::Value state;
    state[Uuid()] = mVariables;

    if (formatted)
    {
        Json::StyledWriter writer;
        return writer.write(state);
    }
    else
    {
        Json::FastWriter writer;
        return writer.write(state);
    }
}

