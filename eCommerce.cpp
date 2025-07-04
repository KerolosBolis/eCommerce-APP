// Updated E-commerce System with Example Use Cases and Detailed Output
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

class Shippable {
public:
    virtual string getName() const = 0;
    virtual double getWeight() const = 0;
    virtual ~Shippable() = default;
};

class Product {
protected:
    string name;
    double price;
    int quantity;

public:
    Product(string name, double price, int quantity)
        : name(name), price(price), quantity(quantity) {}

    virtual ~Product() = default;

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void reduceQuantity(int amount) {
        if (amount > quantity)
            throw runtime_error("Not enough stock.");
        quantity -= amount;
    }

    virtual bool isExpired() const = 0;
    virtual bool isShippable() const = 0;
};

class ExpirableProduct : public Product {
    time_t expiryDate;

public:
    ExpirableProduct(string name, double price, int quantity, time_t expiryDate)
        : Product(name, price, quantity), expiryDate(expiryDate) {}

    bool isExpired() const override {
        return time(nullptr) > expiryDate;
    }

    bool isShippable() const override {
        return true;
    }
};

class ShippableProduct : public Product, public Shippable {
    double weight;

public:
    ShippableProduct(string name, double price, int quantity, double weight)
        : Product(name, price, quantity), weight(weight) {}

    double getWeight() const override { return weight; }
    string getName() const override { return name; }

    bool isExpired() const override { return false; }
    bool isShippable() const override { return true; }
};

class DigitalProduct : public Product {
public:
    DigitalProduct(string name, double price, int quantity)
        : Product(name, price, quantity) {}

    bool isExpired() const override { return false; }
    bool isShippable() const override { return false; }
};

struct CartItem {
    shared_ptr<Product> product;
    int quantity;

    double getTotalPrice() const {
        return product->getPrice() * quantity;
    }
};

class Cart {
    vector<CartItem> items;

public:
    void addToCart(shared_ptr<Product> product, int quantity) {
        if (product->getQuantity() < quantity)
            throw runtime_error("Insufficient stock.");
        items.push_back({product, quantity});
    }

    const vector<CartItem>& getItems() const { return items; }

    bool isEmpty() const { return items.empty(); }

    double calculateSubtotal() const {
        double sum = 0.0;
        for (const auto& item : items)
            sum += item.getTotalPrice();
        return sum;
    }

    double calculateShippingFee() const {
        double fee = 0.0;
        for (const auto& item : items) {
            auto shippable = dynamic_pointer_cast<Shippable>(item.product);
            if (shippable)
                fee += shippable->getWeight() * item.quantity * 10.0; // $10 per kg
        }
        return fee;
    }

    vector<pair<shared_ptr<Shippable>, int>> getShippableItems() const {
        vector<pair<shared_ptr<Shippable>, int>> result;
        for (const auto& item : items) {
            auto shippable = dynamic_pointer_cast<Shippable>(item.product);
            if (shippable)
                result.push_back({shippable, item.quantity});
        }
        return result;
    }
};

class Customer {
    string name;
    double balance;

public:
    Customer(string name, double balance) : name(name), balance(balance) {}

    double getBalance() const { return balance; }

    void deductBalance(double amount) {
        if (balance < amount)
            throw runtime_error("Insufficient balance.");
        balance -= amount;
    }
};

class ShippingService {
public:
    static void ship(const vector<pair<shared_ptr<Shippable>, int>>& items) {
        cout << "\n** Shipment notice **\n";
        double totalWeight = 0.0;
        for (const auto& pair : items) {
    auto item = pair.first;
    int qty = pair.second;
    double weight = item->getWeight() * qty;
    totalWeight += weight;
    cout << qty << "x " << item->getName() << "\t"
         << fixed << setprecision(0) << weight * 1000 << "g\n";
}
        cout << "Total package weight " << fixed << setprecision(1) << totalWeight << "kg\n";
    }
};

class CheckoutService {
public:
    static void checkout(Customer& customer, Cart& cart) {
        if (cart.isEmpty()) throw runtime_error("Cart is empty.");

        for (const auto& item : cart.getItems()) {
            if (item.product->isExpired())
                throw runtime_error(item.product->getName() + " is expired.");
            if (item.product->getQuantity() < item.quantity)
                throw runtime_error(item.product->getName() + " is out of stock.");
        }

        double subtotal = cart.calculateSubtotal();
        double shippingFee = cart.calculateShippingFee();
        double total = subtotal + shippingFee;

        if (customer.getBalance() < total)
            throw runtime_error("Insufficient customer balance.");

        for (const auto& item : cart.getItems())
            item.product->reduceQuantity(item.quantity);

        customer.deductBalance(total);

        auto shippableItems = cart.getShippableItems();
        if (!shippableItems.empty()) {
            ShippingService::ship(shippableItems);
        }

        cout << "\n** Checkout receipt **\n";
        for (const auto& item : cart.getItems()) {
            cout << item.quantity << "x " << item.product->getName() << "\t" << item.getTotalPrice() << "\n";
        }
        cout << "----------------------\n";
        cout << "Subtotal\t" << subtotal << "\n";
        cout << "Shipping\t" << shippingFee << "\n";
        cout << "Amount\t" << total << "\n";
    }
};

int main() {
    time_t tomorrow = time(nullptr) + 86400;

    auto cheese = make_shared<ShippableProduct>("Cheese", 100.0, 10, 0.2);
    auto biscuits = make_shared<ShippableProduct>("Biscuits", 150.0, 5, 0.7);
    auto tv = make_shared<ShippableProduct>("TV", 1000.0, 3, 10.0);
    auto scratchCard = make_shared<DigitalProduct>("Scratch Card", 50.0, 100);


    // First use case (Normal Customer)
   /*Customer customer("Kerolos", 2000);
    Cart cart;

    try {
        cart.addToCart(cheese, 2);
        cart.addToCart(biscuits, 1);
        cart.addToCart(scratchCard, 1);

        CheckoutService::checkout(customer, cart);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
        */
       // Use Case for Insufficient customer balance
    Customer customer("Kerolos", 1000);
    Cart cart;

    try {
        cart.addToCart(cheese, 2);
        cart.addToCart(biscuits, 1);
        cart.addToCart(scratchCard, 1);
        cart.addToCart(tv,1);
        CheckoutService::checkout(customer, cart);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    return 0;
}
