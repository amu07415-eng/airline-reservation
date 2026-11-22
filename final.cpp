#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <fstream>
#include <limits>
using namespace std;

class Booking {
public:
    int id;
    string passengerName;
    int row, col;
    double fare;

    Booking(int i = 0, string n = "", int r = 0, int c = 0, double f = 2500) {
        id = i;
        passengerName = n;
        row = r;
        col = c;
        fare = f;
    }

    friend ostream& operator<<(ostream& out, const Booking& b) {
        out << "ID: " << b.id
            << ", Name: " << b.passengerName
            << ", Seat: " << b.row + 1 << "-" << b.col + 1
            << ", Fare: Rs. " << b.fare;
        return out;
    }
};

class Flight {
    string flightName;
    bool seat[4][3] = {};

public:
    Flight(string name = "AS101") {
        flightName = name;
    }

    bool bookSeat(int& r, int& c) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) {
                if (!seat[i][j]) {
                    seat[i][j] = true;
                    r = i;
                    c = j;
                    return true;
                }
            }
        }
        return false;
    }

    void releaseSeat(int r, int c) {
        if (r >= 0 && r < 4 && c >= 0 && c < 3)
            seat[r][c] = false;
    }

    void showSeats() const {
        cout << "\nFlight: " << flightName << "\n";
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++){
                cout << (seat[i][j] ? "[X] " : "[ ] ");
            }
            cout << "\n";
        }
    }
};

class AirlineSystem {
    Flight flight;
    Booking bookings[12];
    int bookingCount = 0;
    int nextId = 1001;
    queue<string> waitingList;
    stack<int> recentBookings;
    priority_queue<pair<int, string>> boarding;

public:
    void viewSeats() const {
        flight.showSeats();
    }
    void reserve(){
        if (bookingCount == 12) {
            string name;
            cout << "Flight is full. Enter name for waiting list: ";
            getline(cin >> ws, name);
            waitingList.push(name);
            cout << "Added to waiting list.\n";
            return;
        }
        string name;
        cout << "Passenger name: ";
        getline(cin >> ws, name);

        int r, c;
        if (!flight.bookSeat(r, c)) {
            cout << "No seats available.\n";
            return;
        }

        bookings[bookingCount] = Booking(nextId++, name, r, c, 2500);
        recentBookings.push(bookingCount);
        cout << "Booking confirmed!\n";
        cout << bookings[bookingCount] << "\n";
        bookingCount++;
    }

    void showBookings() const {
        if (bookingCount == 0) {
            cout << "No bookings found.\n";
            return;
        }
        for (int i = 0; i < bookingCount; i++){
            cout << bookings[i] << "\n";
    }
    }
    void cancelRecent() {
        if (recentBookings.empty()) {
            cout << "No booking available to cancel.\n";
            return;
        }

        int index = recentBookings.top();
        recentBookings.pop();
        int freedRow = bookings[index].row;
        int freedCol = bookings[index].col;
        flight.releaseSeat(freedRow,freedCol);
        cout << "Cancelled booking for " << bookings[index].passengerName << ".\n";

        // Remove the booking from the active list.
        for (int i = index; i < bookingCount - 1; i++){
            bookings[i] = bookings[i + 1];
        }
        bookingCount--;

        // Rebuild history indices after shifting the array.
        stack<int> updated;
        while (!recentBookings.empty()) {
            int oldIndex = recentBookings.top();
            recentBookings.pop();
            if (oldIndex > index)
                oldIndex--;
            updated.push(oldIndex);
        }
        while (!updated.empty()) {
            recentBookings.push(updated.top());
            updated.pop();
        }
          if(!waitingList.empty()){
              string nextPassenger=waitingList.front();
              waitingList.pop();
              int r,c;
              r=freedRow;
              c=freedCol;
              bookings[bookingCount]=Booking(nextId++, nextPassenger,r,c,2500);
              recentBookings.push(bookingCount);
              cout<< "Seat autmtically assigned to waiting list passenger: "<<bookings[bookingcount] << "\n";
              bookingCount++;
          }            
    }
    void addBoardingPriority() {
        string name;
        int priority;
        cout << "Passenger name: ";
        getline(cin >> ws, name);
        cout << "Enter priority (higher number = higher priority): ";
        while (!(cin >> priority)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cout << "invalid input . please enter a numeric priority: ";
        }
        boarding.push({priority, name});
        cout << "Passenger added to boarding queue.\n";
    }

    void nextToBoard() {
        if (boarding.empty()) {
            cout << "No passengers in boarding queue.\n";
            return;
        }
        cout << "Next passenger: " << boarding.top().second << "(priority: "<< boarding.top().first << ") \n";
        boarding.pop();
    }

    void showWaiting() {
        if (waitingList.empty()) {
            cout << "Waiting list is empty.\n";
            return;
        }

        queue<string> temp = waitingList;
        cout << "\nWaiting passengers:\n";
        while (!temp.empty()) {
            cout <<"-" << temp.front() << "\n";
            temp.pop();
        }
    }

    void saveBookings() {
        ofstream file("aerosync.txt");
        if (!file) {
            cout << "Could not open file.\n";
            return;
        }
        for (int i = 0; i < bookingCount; i++)
            file << bookings[i].id << ","
                 << bookings[i].passengerName << ","
                 << bookings[i].row + 1 << ","
                 << bookings[i].col + 1 << ","
                 << bookings[i].fare << "\n";

        file.close();
        cout << "Bookings saved.\n";
    }
};

int main() {
    AirlineSystem system;
    int choice;
    do {
        cout << "\n===== AEROSYNC =====\n";
        cout << "1. View seat map\n";
        cout << "2. Reserve a seat\n";
        cout << "3. View bookings\n";
        cout << "4. Cancel most recent booking\n";
        cout << "5. View waiting list\n";
        cout << "6. Add boarding priority\n";
        cout << "7. Board next passenger\n";
        cout << "8. Save bookings\n";
        cout << "0. Exit\n";
        cout << "Choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: system.viewSeats(); break; // See note below
            case 2: system.reserve(); break;
            case 3: system.showBookings(); break;
            case 4: system.cancelRecent(); break;
            case 5: system.showWaiting(); break;
            case 6: system.addBoardingPriority(); break;
            case 7: system.nextToBoard(); break;
            case 8: system.saveBookings(); break;
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}
