#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <random>
#include <sstream>

// Interface for the deposit strategy (Polymorphism used here)
class IDeposit {
public:
    virtual double calculateDeposit(double amount) const = 0; // Pure virtual function
    virtual ~IDeposit() {} // Virtual destructor for proper cleanup
};

// Exception class for handling invalid inputs
class InvalidInputException : public std::exception {
private:
    std::string message;
public:
    explicit InvalidInputException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Exception class for handling negative deposit amounts
class NegativeDepositException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Deposit amount cannot be negative";
    }
};

// Concrete strategy class for FixedDeposit
class FixedDeposit : public IDeposit {
public:
    double calculateDeposit(double amount) const override {
        if (amount > 1000000) {
            throw InvalidInputException("The maximum deposit amount for the fixed account is 1,000,000. Please deposit less.");
        }
        return amount + 100; // Fixed deposit adds 100 to the deposit
    }
};

// Concrete strategy class for NormalDeposit
class NormalDeposit : public IDeposit {
public:
    double calculateDeposit(double amount) const override {
        return amount; // No additional amount is added in NormalDeposit
    }
};

// Function to validate deposit amount (numeric and non-negative)
void validateDepositAmount(double amount) {
    if (amount < 0) {
        throw NegativeDepositException();
    }
}

// Function to check if a string contains only alphabetic characters
bool isValidName(const std::string& name) {
    for (char c : name) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

// Function to check if the input is numeric
bool isNumeric(const std::string& str) {
    char* end = nullptr;
    std::strtod(str.c_str(), &end);
    return end != str.c_str() && *end == '\0'; // Ensure the whole string is a number
}

// Function to generate a random 6-digit number
std::string generateRandomID() {
    std::random_device rd;  // Seed for the random number generator
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine
    std::uniform_int_distribution<> dist(100000, 999999); // Range for six-digit number

    std::stringstream ss;
    ss << "PZ" << dist(gen); // Combine "PZ" with the random number
    return ss.str();
}

// Depositor class to hold information about a depositor
class Depositor {
private:
    std::string name;
    double amount;
    const IDeposit* depositStrategy;
    std::string depositorID; // String for ID in format PZxxxxxx

public:
    Depositor(const std::string& id, const std::string& name, double amount, const IDeposit* strategy)
        : depositorID(id), name(name), amount(amount), depositStrategy(strategy) {}

    double getDepositAmount() const {
        return depositStrategy->calculateDeposit(amount);
    }

    std::string getName() const {
        return name;
    }

    std::string getID() const {
        return depositorID;
    }

    void deposit(double amount) {
        validateDepositAmount(amount);
        this->amount += depositStrategy->calculateDeposit(amount); // Add to the deposit amount using strategy
    }
};

// Bank class to manage depositors and calculate total deposits
class Bank {
private:
    std::vector<Depositor> depositors;

public:
    void addDepositor(const std::string& name, const IDeposit* strategy) {
        std::string depositorID = generateRandomID(); // Generate a random ID
        depositors.emplace_back(depositorID, name, 0, strategy); // Add depositor with 0 initial deposit

        // Print the new depositor's ID immediately after adding
        std::cout << "Depositor added successfully! User ID: " << depositorID << "\n";
    }

    bool depositToAccount(const std::string& depositorID, double amount) {
        for (auto& depositor : depositors) {
            if (depositor.getID() == depositorID) {
                try {
                    depositor.deposit(amount); // Deposit the amount to the found account
                    std::cout << "Deposit of " << amount << " made to account ID: " << depositorID << "\n";
                }
                catch (const InvalidInputException& e) {
                    std::cerr << "Error: " << e.what() << "\n";
                }
                return true;
            }
        }
        return false; // If no depositor matches the given ID
    }

    double calculateTotalDeposits() const {
        double total = 0;
        for (const auto& depositor : depositors) {
            total += depositor.getDepositAmount();
        }
        return total;
    }

    void listDepositors() const {
        if (depositors.empty()) {
            std::cout << "No depositors were added.\n";
            return;
        }

        std::cout << "\nList of depositors:\n";
        for (const auto& depositor : depositors) {
            std::cout << "Depositor ID: " << depositor.getID()
                << ", Name: " << depositor.getName()
                << ", Deposit Amount: " << depositor.getDepositAmount() << std::endl;
        }
    }
};

// Helper function to get valid depositor name
std::string getValidDepositorName() {
    std::string name;
    while (true) {
        std::cout << "Enter depositor name (letters only): ";
        std::cin >> name;
        if (isValidName(name)) {
            break;
        }
        else {
            std::cerr << "Invalid name. Only letters are allowed. Please try again.\n";
        }
    }
    return name;
}

// Helper function to get valid deposit amount
double getValidDepositAmount() {
    std::string amountStr;
    double amount;
    while (true) {
        std::cout << "Enter deposit amount: ";
        std::cin >> amountStr;
        if (isNumeric(amountStr)) {
            amount = std::stod(amountStr);
            if (amount >= 0) {
                break;
            }
            else {
                std::cerr << "Amount cannot be negative. Please try again.\n";
            }
        }
        else {
            std::cerr << "Invalid amount. Please enter a numeric value.\n";
        }
    }
    return amount;
}

// Main function to interact with the user
int main() {
    Bank bank;
    std::string choice;
    try {
        while (true) {
            std::cout << "\nSelect an option:\n";
            std::cout << "1. Add Depositor\n";
            std::cout << "2. List Depositors\n";
            std::cout << "3. View Total Deposits\n";
            std::cout << "4. Deposit Amount\n"; // The only way to deposit
            std::cout << "5. Exit\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            if (choice == "1") {
                std::string name = getValidDepositorName();
                int strategyChoice;
                const IDeposit* strategy = nullptr;
                while (true) {
                    std::cout << "Choose deposit strategy (1: Normal, 2: Fixed): ";
                    std::cin >> strategyChoice;
                    if (strategyChoice == 1) {
                        strategy = new NormalDeposit();
                        break;
                    }
                    else if (strategyChoice == 2) {
                        strategy = new FixedDeposit();
                        break;
                    }
                    else {
                        std::cerr << "Invalid strategy choice. Please try again.\n";
                    }
                }

                bank.addDepositor(name, strategy);

            }
            else if (choice == "2") {
                bank.listDepositors();
            }
            else if (choice == "3") {
                double totalDeposits = bank.calculateTotalDeposits();
                if (totalDeposits == 0) {
                    std::cout << "No deposits have been made yet.\n";
                }
                else {
                    std::cout << "Total deposits: " << totalDeposits << std::endl;
                }
            }
            else if (choice == "4") {
                std::string depositorID;
                std::cout << "Enter depositor ID to deposit to: ";
                std::cin >> depositorID;

                double amount = getValidDepositAmount();

                if (!bank.depositToAccount(depositorID, amount)) {
                    std::cerr << "No depositor found with the ID: " << depositorID << "\n";
                }

            }
            else if (choice == "5") {
                std::cout << "Exiting program.\n";
                break;
            }
            else {
                std::cerr << "Invalid choice. Please try again.\n";
            }
        }

    }
    catch (const InvalidInputException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (const NegativeDepositException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
