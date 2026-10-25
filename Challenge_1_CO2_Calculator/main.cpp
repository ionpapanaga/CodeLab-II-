#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <iomanip>

// Function to prompt and validate the distance travelled
double getDistanceTravelled() {
    double distance = 0.0;
    while (true) {
        std::cout << "Enter journey distance in miles: ";
        if (std::cin >> distance && distance >= 0.0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return distance;
        }
        std::cout << "Invalid input. Please enter a non-negative number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Function to prompt and validate the fuel type
char getFuelType() {
    char fuel = ' ';
    while (true) {
        std::cout << "Enter fuel type (P for Petrol, D for Diesel): ";
        std::string input;
        std::getline(std::cin, input);

        if (input.length() == 1) {
            fuel = std::toupper(static_cast<unsigned char>(input[0]));
            if (fuel == 'P' || fuel == 'D') {
                return fuel;
            }
        }
        std::cout << "Invalid selection. Please enter 'P' or 'D'.\n";
    }
}

// Function to calculate CO2 emissions based on distance and fuel type
double calculateEmissions(double distance, char fuelType) {
    const double FUEL_RATE_PER_100_MILES = 9.66; // 9.66 litres per 100 miles
    const double DIESEL_CO2_PER_LITRE = 2.68;    // kg CO2 per litre
    const double PETROL_CO2_PER_LITRE = 2.31;    // kg CO2 per litre

    double litresUsed = (distance / 100.0) * FUEL_RATE_PER_100_MILES;
    double emissionFactor = (fuelType == 'D') ? DIESEL_CO2_PER_LITRE : PETROL_CO2_PER_LITRE;

    return litresUsed * emissionFactor;
}

int main() {
    std::cout << "====================================\n";
    std::cout << "        Car CO2 Calculator          \n";
    std::cout << "====================================\n\n";

    double distance = getDistanceTravelled();
    char fuelType = getFuelType();

    double totalCO2 = calculateEmissions(distance, fuelType);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n--- Journey Summary ---\n";
    std::cout << "Distance: " << distance << " miles\n";
    std::cout << "Fuel Type: " << ((fuelType == 'D') ? "Diesel" : "Petrol") << "\n";
    std::cout << "Estimated CO2 Emissions: " << totalCO2 << " kg\n";

    return 0;
}