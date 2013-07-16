/*
 * main.cpp
 *
 *  Created on: Jan 18, 2012
 *      Author: david
 */

#include <iostream>
#include <cdbapp/ApplicationDatabaseInterface.h>
#include <cdbapp/State.h>

State state1, state2;

void
PrintStates()
{
    std::cout << "================================" << std::endl;
    std::cout << state1.Write(true) << std::endl;
    std::cout << state2.Write(true) << std::endl;
    std::cout << "================================" << std::endl << std::endl;
}

int main(void)
{
    ApplicationDatabaseInterface collab_srvr("tcp://localhost:5570");


    std::cout.precision(15);

    state1.SetVariable("val1", false);
    state1.SetVariable("val2", "oh hi");

    state2.SetVariable("val1", "not uno dos tres");
    std::vector<double> vec; vec.push_back(9.9); vec.push_back(8.8); vec.push_back(6.6);
    state2.SetVariable("vec3", vec);

    PrintStates();

    std::list< std::string > states;
    states.push_back(state1.Write());
    states.push_back(state2.Write());

    collab_srvr.SendSaveMsg(states);

    while (1)
    {
        uint64_t time_frame_back = 0;
        std::list< std::string > states_back;
        collab_srvr.SendLoadMsg(0, time_frame_back, states_back);

        if (time_frame_back > 0) // oh sweet, we got stuff back
        {
            std::cout << "Timestamp from server: " << time_frame_back << std::endl;

            // Update states
            std::list< std::string >::iterator it;
            for (it = states_back.begin(); it != states_back.end(); it++)
                {
                    State s = State::Read(*it);
                    if (s.Uuid() == state1.Uuid())
                        state1.Variables(s.Variables());
                    else if (s.Uuid() == state2.Uuid())
                        state2.Variables(s.Variables());
                }

            // Print out the changed values and bail out
            PrintStates();
            break;
        }
    }

    collab_srvr.Disconnect();

    return 0;
}
