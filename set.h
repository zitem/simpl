#pragma once

namespace node {
struct Fact;
class Module;
} // namespace node
namespace set {

struct ISet {
    template <typename T> [[nodiscard]] T &cast() { return *static_cast<T *>(this); }
};

class Number : public ISet {
public:
    Number(int val) : _val(val) {}
    Number(std::string const &str) : _val(std::stoi(str)) {}
    Number operator+(Number const &rhs) const { return _val + rhs._val; }
    Number operator-(Number const &rhs) const { return _val - rhs._val; }
    Number operator*(Number const &rhs) const { return _val * rhs._val; }
    Number operator/(Number const &rhs) const { return _val / rhs._val; }
    [[nodiscard]] int value() const { return _val; }
private:
    int _val;
};
struct Module : public ISet {
    Module(std::string name) : name(std::move(name)) {}
    std::string name;
    std::multimap<std::string, node::Fact *> facts;
    set::Module operator+(set::Module const &rhs) const {
        set::Module r(*this);
        r.facts.insert_range(rhs.facts);
        return r;
    }
    bool erase(std::string const &lhs) {
        facts.erase(lhs);
        return facts.empty();
    }
};
struct Sets : ISet, std::map<std::string, std::unique_ptr<ISet>> {
    Sets(node::Module const *module = nullptr) : module(module) {}
    node::Module const *module;
};

} // namespace set