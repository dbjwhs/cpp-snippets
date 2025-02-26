// MIT License
// Copyright (c) 2025 dbjwhs

#include <string>
#include "../../../../headers/project_utils.hpp"

// abstract base class defining the template method for beverage preparation
class BeverageMaker {
protected:
    Logger& m_logger = Logger::getInstance();
    std::string m_beverageName;
    double m_temperature;
    bool m_needsSweetener;

    // primitive operations to be implemented by concrete classes
    virtual void heatWater() = 0;               // pure virtual function
    virtual void addMainIngredient() = 0;       // pure virtual function
    virtual void pourInCup() = 0;               // pure virtual function

    // hook methods with default implementations
    virtual bool customerWantsSweetener() {
        return m_needsSweetener;
    }

    virtual void addSweetener() {
        if (customerWantsSweetener()) {
            m_logger.log(LogLevel::INFO, "adding sweetener to " + m_beverageName);
        }
    }

    virtual void addToppings() {
        // default empty implementation
    }

public:
    BeverageMaker(std::string  name, const double temp) :
        m_beverageName(std::move(name)),
        m_temperature(temp),
        m_needsSweetener(false) {}

    virtual ~BeverageMaker() = default;

    // template method defining the algorithm skeleton
    void prepareBeverage() {
        m_logger.log(LogLevel::INFO, "starting preparation of " + m_beverageName + "...");

        // fixed sequence of steps
        heatWater();
        addMainIngredient();
        pourInCup();
        addSweetener();
        addToppings();

        m_logger.log(LogLevel::INFO, m_beverageName + " is ready to serve!");
    }

    // public method to set sweetener preference
    void setSweetenerPreference(const bool needsSweetener) {
        m_needsSweetener = needsSweetener;
    }
};

// concrete class for preparing coffee
class CoffeeMaker final : public BeverageMaker {
private:
    Logger& m_logger = Logger::getInstance();
    bool m_needsFoamedMilk;

public:
    CoffeeMaker() :
        BeverageMaker("Coffee", 95.0), // coffee at 95°c
        m_needsFoamedMilk(false) {}

    void setFoamedMilk(const bool need) {
        m_needsFoamedMilk = need;
    }

protected:
    void heatWater() override {
        m_logger.log(LogLevel::INFO, "heating water to " + std::to_string(m_temperature) + "°F for coffee");
    }

    void addMainIngredient() override {
        m_logger.log(LogLevel::INFO, "brewing coffee grounds in filter");
    }

    void pourInCup() override {
        m_logger.log(LogLevel::INFO, "pouring coffee through filter into cup");
    }

    void addToppings() override {
        if (m_needsFoamedMilk) {
            m_logger.log(LogLevel::INFO, "adding foamed milk for latte");
        }
    }
};

// concrete class for preparing tea
class TeaMaker final : public BeverageMaker {
private:
    Logger& m_logger = Logger::getInstance();
    std::string m_teaType;

public:
    explicit TeaMaker(const std::string& teaType) : BeverageMaker(teaType + " Tea", 125.0),
        m_teaType(teaType) {}

protected:
    void heatWater() override {
        m_logger.log(LogLevel::INFO, "heating water to " + std::to_string(m_temperature)
            + "°F for " + m_teaType + " tea");
    }

    void addMainIngredient() override {
        m_logger.log(LogLevel::INFO, "steeping " + m_teaType + " tea bag");
    }

    void pourInCup() override {
        m_logger.log(LogLevel::INFO, "pouring hot tea into cup");
    }

    // optional: override sweetener method to suggest honey for tea
    void addSweetener() override {
        if (customerWantsSweetener()) {
            m_logger.log(LogLevel::INFO, "adding honey to " + m_beverageName);
        }
    }
};

// concrete class for preparing hot chocolate
class HotChocolateMaker : public BeverageMaker {
private:
    Logger& m_logger = Logger::getInstance();
    bool m_extraChocolate;

public:
    HotChocolateMaker() :
        BeverageMaker("Hot Chocolate", 130.0), m_extraChocolate(false) {}

    void setExtraChocolate(const bool extra) {
        m_extraChocolate = extra;
    }

protected:
    void heatWater() override {
        m_logger.log(LogLevel::INFO, "heating milk to " + std::to_string(m_temperature) + "°F");
    }

    void addMainIngredient() override {
        m_logger.log(LogLevel::INFO, "mixing in chocolate powder");
        if (m_extraChocolate) {
            m_logger.log(LogLevel::INFO, "adding extra chocolate powder");
        }
    }

    void pourInCup() override {
        m_logger.log(LogLevel::INFO, "pouring hot chocolate into cup");
    }

    void addToppings() override {
        m_logger.log(LogLevel::INFO, "adding whipped cream and chocolate sprinkles");
    }
};

// main function with comprehensive testing
int main() {
    Logger& logger = Logger::getInstance();

    logger.log(LogLevel::INFO, "=== beverage preparation template method pattern demonstration ===");

    // test 1: prepare coffee with different options
    logger.log(LogLevel::INFO, "test 1 - coffee preparation variations:");
    CoffeeMaker coffee;
    coffee.prepareBeverage();  // plain coffee

    coffee.setSweetenerPreference(true);
    coffee.setFoamedMilk(true);
    coffee.prepareBeverage();  // sweet latte

    // test 2: prepare different types of tea
    logger.log(LogLevel::INFO, "test 2 - different tea preparations:");
    TeaMaker greenTea("Green");
    greenTea.prepareBeverage();  // plain green tea

    TeaMaker earlGrey("Earl Grey");
    earlGrey.setSweetenerPreference(true);
    earlGrey.prepareBeverage();  // sweet earl grey tea

    // test 3: prepare hot chocolate
    logger.log(LogLevel::INFO, "test 3 - hot chocolate variations:");
    HotChocolateMaker hotChocolate;
    hotChocolate.prepareBeverage();  // regular hot chocolate

    hotChocolate.setExtraChocolate(true);
    hotChocolate.setSweetenerPreference(true);
    hotChocolate.prepareBeverage();  // extra chocolatey and sweet

    // test 4: demonstrate polymorphic behavior
    logger.log(LogLevel::INFO, "test 4 - polymorphic beverage preparation:");
    BeverageMaker* beverages[] = {
        new CoffeeMaker(),
        new TeaMaker("Chamomile"),
        new HotChocolateMaker()
    };

    for (auto beverage : beverages) {
        beverage->prepareBeverage();
        delete beverage;
    }
    return 0;
}
