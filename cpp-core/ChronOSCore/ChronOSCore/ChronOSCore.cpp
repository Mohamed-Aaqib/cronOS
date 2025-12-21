#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include "core/RingBuffer.h"
#include "core/PCAPParser.h"
#include "events/Event.h"

using namespace std;

int main()
{
    cout << " this does work;!" << std::endl;
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;
    RingBuffer<Event, 8192> buffer;


    try
    {
        PCAPParser pcap("data/http-chunked-gzip.pcap");
        pcap.parse(buffer);

        //driver = get_driver_instance();
        //con = driver->connect("tcp://127.0.0.1:3306", "root", "");
        //cout << " this works \n";
        std::shared_ptr<Event> evt;
        while (buffer.pop(evt)) {
            std::cout << evt->serialize() << std::endl;
        }

        std::cout << "Parsed events: " << buffer.getSize() << std::endl;
        
    }
    catch (sql::SQLException e)
    {
        //cout << "Could not connect to server. Error message: " << e.what() << endl;
        //system("pause");
        cout << "couldn't open file please try again \n";
        exit(1);
    }

    return 0;
}

