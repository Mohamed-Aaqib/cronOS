#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
// need to fix the path
#include "utils/test_pcap_ingestion.h"
#include "core/test_nodes_edges.h"
#include "core/test_path_reconstructor.h"
#include "events/Event.h"

using namespace std;

int main()
{
    //cout << " this does work;!" << std::endl;
    //sql::Driver* driver;
    //sql::Connection* con;
    //sql::Statement* stmt;
    //sql::PreparedStatement* pstmt;


    //try
    //{

    //    //driver = get_driver_instance();
    //    //con = driver->connect("tcp://127.0.0.1:3306", "root", "");
    //    //cout << " this works \n";
    //    std::shared_ptr<Event> evt;
    //    
    //}
    //catch (sql::SQLException e)
    //{
    //    //cout << "Could not connect to server. Error message: " << e.what() << endl;
    //    //system("pause");
    //    cout << "couldn't open file please try again \n";
    //    exit(1);
    //}

    cout << "=== Running test_pcap_ingestion ===\n";
    test_pcap_ingestion();
    
    //cout << "\n=== Running test_nodes_edges ===\n";
    //test_nodes_edges();
    //
    //cout << "\n=== Running test_path_reconstructor ===\n";
    //test_path_reconstructor();

    return 0;
}

