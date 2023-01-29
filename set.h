#pragma once

namespace node {
struct Fact;
class Module;
} // namespace node
 
namespace set {

class ISet {
public:
    ISet(ISet *superset) : pointer(superset) {}
    virtual ~ISet() = default;
    [[nodiscard]] virtual size_t range() const { return 0; }
    [[nodiscard]] virtual std::string show() const { return "interface"; }
    [[nodiscard]] virtual std::unique_ptr<ISet> clone() const { return std::make_unique<ISet>(*this); }
    [[nodiscard]] virtual ISet *superset() const { return pointer; }
    [[nodiscard]] virtual ISet *thisset() const { return pointer; }

    [[nodiscard]] virtual std::unique_ptr<ISet> operator+(ISet const &set) const { return (void)set, nullptr; }
    [[nodiscard]] virtual std::unique_ptr<ISet> operator-(ISet const &set) const { return (void)set, nullptr; }
    [[nodiscard]] virtual std::unique_ptr<ISet> operator*(ISet const &set) const { return (void)set, nullptr; }
    [[nodiscard]] virtual std::unique_ptr<ISet> operator/(ISet const &set) const { return (void)set, nullptr; }
    [[nodiscard]] virtual std::unique_ptr<ISet> operator|(ISet const &set) const { return (void)set, nullptr; }

    // [[nodiscard]] virtual std::unique_ptr<ISet> belongs(ISet const &set, ISet *super) const;
    [[nodiscard]] virtual std::unique_ptr<ISet> contains(ISet const &set, ISet *super) const {
        return equal(*set.superset(), super);
    }
    [[nodiscard]] virtual std::unique_ptr<ISet> equal(ISet const &set, ISet *super) const {
        return (void)set, (void)super, nullptr;
    }

    template <typename T> [[nodiscard]] T &cast() { return *static_cast<T *>(this); }
    template <typename T> [[nodiscard]] T const &cast() const { return *static_cast<T const *>(this); }
protected:
    ISet *pointer{};
};

template <typename T> struct Base : public ISet {
    Base(T val, ISet *superset) : val(val), ISet(superset) {}

    template <auto BinOp>
    [[nodiscard]] std::unique_ptr<ISet> binOp(ISet const &set, ISet *newSuperset = nullptr) const {
        if (superset() != set.superset()) return nullptr;
        auto res = BinOp(val, set.cast<Base<T>>().val);
        return std::make_unique<Base<decltype(res)>>(res, newSuperset ? newSuperset : pointer);
    }

    [[nodiscard]] std::unique_ptr<ISet> equal(ISet const &set, ISet *super) const override {
        return binOp<[](auto lhs, auto rhs) { return lhs == rhs; }>(set, super);
    }

    [[nodiscard]] std::unique_ptr<ISet> operator+(ISet const &set) const override {
        return binOp<[](auto lhs, auto rhs) { return lhs + rhs; }>(set);
    }

    [[nodiscard]] std::unique_ptr<ISet> operator-(ISet const &set) const override {
        return binOp<[](auto lhs, auto rhs) { return lhs - rhs; }>(set);
    }

    [[nodiscard]] std::unique_ptr<ISet> operator*(ISet const &set) const override {
        return binOp<[](auto lhs, auto rhs) { return lhs * rhs; }>(set);
    }

    [[nodiscard]] std::unique_ptr<ISet> operator/(ISet const &set) const override {
        return binOp<[](auto lhs, auto rhs) { return lhs / rhs; }>(set);
    }

    [[nodiscard]] T const &value() const { return val; }

    [[nodiscard]] std::string show() const override {
        if constexpr (std::is_same_v<bool, decltype(val)>) {
            return val ? "true" : "false";
        }
        return std::to_string(val);
    }

    [[nodiscard]] std::unique_ptr<ISet> clone() const override { return std::make_unique<Base<T>>(*this); }

protected:
    T val;
};

using Bool = Base<bool>;
using Int = Base<int>;

struct Identity : public ISet {
    Identity() : ISet(nullptr) {}
    [[nodiscard]] std::unique_ptr<ISet> equal(ISet const &set, ISet *super) const override {
        return std::make_unique<Bool>(this == &set, super);
    }
};

struct Sets : ISet, std::map<std::string, std::unique_ptr<ISet>> {
    Sets(node::Module const *module = nullptr, ISet *superset = nullptr) : ISet(superset), module(module) {}
    node::Module const *module;
};

struct Ref : ISet {
    Ref(ISet *set) : ISet(set) {}
    [[nodiscard]] std::unique_ptr<ISet> operator+(ISet const &set) const override { return *pointer + set; }
    [[nodiscard]] std::unique_ptr<ISet> operator-(ISet const &set) const override { return *pointer - set; }
    [[nodiscard]] std::unique_ptr<ISet> operator*(ISet const &set) const override { return *pointer * set; }
    [[nodiscard]] std::unique_ptr<ISet> operator/(ISet const &set) const override { return *pointer / set; }
    [[nodiscard]] std::unique_ptr<ISet> operator|(ISet const &set) const override { return *pointer | set; }
    [[nodiscard]] std::unique_ptr<ISet> equal(ISet const &set, ISet *super) const override {
        return pointer->equal(set, super);
    }
    [[nodiscard]] ISet *thisset() const override { return pointer; }
    [[nodiscard]] ISet *superset() const override { return pointer->superset(); }
    [[nodiscard]] std::string show() const override { return pointer->show(); }
};

struct Universe : ISet {
    Universe() : ISet(nullptr) {}
    std::unique_ptr<ISet> contains(const ISet &set, ISet *super) const override {
        return (void)set, std::make_unique<Bool>(true, super);
    }
};

struct Module : public ISet {
    Module(std::string name) : ISet(nullptr), name(std::move(name)) {}
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

} // namespace set