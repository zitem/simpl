#pragma once

namespace node {
struct Fact;
class Module;
} // namespace node

namespace set {

struct ISet {
    virtual ~ISet() = default;

    virtual ISet const *thisset() const = 0;
    virtual ISet const *superset() const = 0;

    virtual std::unique_ptr<ISet> operator+(ISet const &set) const = 0;
    virtual std::unique_ptr<ISet> operator-(ISet const &set) const = 0;
    virtual std::unique_ptr<ISet> operator*(ISet const &set) const = 0;
    virtual std::unique_ptr<ISet> operator/(ISet const &set) const = 0;
    virtual std::unique_ptr<ISet> operator!() const = 0;

    virtual std::unique_ptr<ISet> lt(ISet const &set, ISet *boolSet) const = 0;
    virtual std::unique_ptr<ISet> gt(ISet const &set, ISet *boolSet) const = 0;
    virtual std::unique_ptr<ISet> lteq(ISet const &set, ISet *boolSet) const = 0;
    virtual std::unique_ptr<ISet> gteq(ISet const &set, ISet *boolSet) const = 0;

    virtual std::unique_ptr<ISet> equals(ISet const &set, ISet *boolSet) const = 0;
    virtual std::unique_ptr<ISet> noteq(ISet const &set, ISet *boolSet) const = 0;
    virtual std::unique_ptr<ISet> contains(ISet const &set, ISet *boolSet) const = 0;

    virtual std::unique_ptr<ISet> clone() const = 0;
    virtual std::string show() const = 0;

    template <typename T> T &cast() { return *static_cast<T *>(this); }
    template <typename T> T const &cast() const { return *static_cast<T const *>(this); }
};

template <typename T> class Base : public ISet {
    constexpr static bool isBool = std::is_same_v<bool, T>;

public:
    Base(T val, ISet *superset) : _val(val), _superset(superset) {}

    ISet const *thisset() const override { return this; }

    ISet const *superset() const override { return _superset; }

    std::unique_ptr<ISet> operator+(ISet const &set) const override {
        if constexpr (isBool) {
            return nullptr;
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs + rhs; }>(set);
        }
    }

    std::unique_ptr<ISet> operator-(ISet const &set) const override {
        if constexpr (isBool) {
            return nullptr;
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs - rhs; }>(set);
        }
    }

    std::unique_ptr<ISet> operator*(ISet const &set) const override {
        if constexpr (isBool) {
            return nullptr;
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs * rhs; }>(set);
        }
    }

    std::unique_ptr<ISet> operator/(ISet const &set) const override {
        if constexpr (isBool) {
            return nullptr;
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs / rhs; }>(set);
        }
    }

    std::unique_ptr<ISet> operator!() const override {
        if constexpr (isBool) {
            return std::make_unique<Base<bool>>(!_val, _superset);
        } else {
            return nullptr;
        }
    }

    std::unique_ptr<ISet> lt(ISet const &set, ISet *boolSet) const override {
        if constexpr (isBool) {
            return nullptr;
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs < rhs; }>(set, boolSet);
        }
    }

    std::unique_ptr<ISet> gt(ISet const &set, ISet *boolSet) const override {
        if constexpr (isBool) {
            return nullptr;
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs > rhs; }>(set, boolSet);
        }
    }

    std::unique_ptr<ISet> lteq(ISet const &set, ISet *boolSet) const override {
        if constexpr (isBool) {
            return nullptr;
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs <= rhs; }>(set, boolSet);
        }
    }

    std::unique_ptr<ISet> gteq(ISet const &set, ISet *boolSet) const override {
        if constexpr (isBool) {
            return nullptr;
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs >= rhs; }>(set, boolSet);
        }
    }

    std::unique_ptr<ISet> equals(ISet const &set, ISet *boolSet) const override {
        return _binOp<[](auto lhs, auto rhs) { return lhs == rhs; }>(set, boolSet);
    }

    std::unique_ptr<ISet> noteq(ISet const &set, ISet *boolSet) const override {
        return _binOp<[](auto lhs, auto rhs) { return lhs != rhs; }>(set, boolSet);
    }

    std::unique_ptr<ISet> contains(ISet const &set, ISet *boolSet) const override {
        return equals(*set.superset(), boolSet);
    }

    std::unique_ptr<ISet> clone() const override { return std::make_unique<Base<T>>(*this); }

    std::string show() const override {
        if constexpr (isBool) {
            return _val ? "true" : "false";
        } else {
            return std::to_string(_val);
        }
    }

    T const &value() const { return _val; }

private:
    template <auto BinOp> std::unique_ptr<ISet> _binOp(ISet const &set, ISet *newSuperset = nullptr) const {
        if (superset() != set.superset()) return nullptr;
        auto res = BinOp(_val, set.cast<Base<T>>()._val);
        return std::make_unique<Base<decltype(res)>>(res, newSuperset ? newSuperset : _superset);
    }

private:
    T _val;
    ISet *_superset{};
};

using Bool = Base<bool>;
using Int = Base<int>;

struct Identity : ISet {
    Identity() = default;

    ISet const *thisset() const override { return this; }
    ISet const *superset() const override { return nullptr; }

    std::unique_ptr<ISet> operator+(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator-(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator*(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator/(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator!() const override { return nullptr; }

    std::unique_ptr<ISet> lt(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> gt(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> lteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> gteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }

    std::unique_ptr<ISet> equals(ISet const &set, ISet *boolSet) const override {
        return std::make_unique<Bool>(this == &set, boolSet);
    }

    std::unique_ptr<ISet> noteq(ISet const &set, ISet *boolSet) const override {
        return std::make_unique<Bool>(this != &set, boolSet);
    }

    std::unique_ptr<ISet> contains(ISet const &set, ISet *boolSet) const override {
        return equals(*set.superset(), boolSet);
    }

    std::unique_ptr<ISet> clone() const override;

    std::string show() const override { return "identity"; }
};

class Ref : public ISet {
public:
    Ref(ISet const *set) : _set(*set->thisset()) {}
    ISet const *thisset() const override { return &_set; }
    ISet const *superset() const override { return _set.superset(); }

    std::unique_ptr<ISet> operator+(ISet const &set) const override { return _set + set; }
    std::unique_ptr<ISet> operator-(ISet const &set) const override { return _set - set; }
    std::unique_ptr<ISet> operator*(ISet const &set) const override { return _set * set; }
    std::unique_ptr<ISet> operator/(ISet const &set) const override { return _set / set; }
    std::unique_ptr<ISet> operator!() const override { return !_set; }

    std::unique_ptr<ISet> lt(ISet const &set, ISet *boolSet) const override { return _set.lt(set, boolSet); }
    std::unique_ptr<ISet> gt(ISet const &set, ISet *boolSet) const override { return _set.gt(set, boolSet); }
    std::unique_ptr<ISet> lteq(ISet const &set, ISet *boolSet) const override { return _set.lteq(set, boolSet); }
    std::unique_ptr<ISet> gteq(ISet const &set, ISet *boolSet) const override { return _set.gteq(set, boolSet); }

    std::unique_ptr<ISet> equals(ISet const &set, ISet *boolSet) const override { return _set.equals(set, boolSet); }
    std::unique_ptr<ISet> noteq(ISet const &set, ISet *boolSet) const override { return _set.noteq(set, boolSet); }
    std::unique_ptr<ISet> contains(ISet const &set, ISet *boolSet) const override {
        return _set.contains(set, boolSet);
    }

    std::unique_ptr<ISet> clone() const override { return std::make_unique<Ref>(&_set); }
    std::string show() const override { return _set.show(); }

private:
    ISet const &_set;
};

struct Universe : ISet {
    ISet const *thisset() const override { return this; }
    ISet const *superset() const override { return nullptr; }

    std::unique_ptr<ISet> operator+(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator-(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator*(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator/(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator!() const override;

    std::unique_ptr<ISet> lt(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> gt(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> lteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> gteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }

    std::unique_ptr<ISet> equals(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> noteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> contains(const ISet & /*set*/, ISet *boolSet) const override {
        return std::make_unique<Bool>(true, boolSet);
    }

    std::unique_ptr<ISet> clone() const override { return std::make_unique<Ref>(this); }
    std::string show() const override { return "universe"; }
};

struct Void : ISet {
    ISet const *thisset() const override { return this; }
    ISet const *superset() const override { return nullptr; }

    std::unique_ptr<ISet> operator+(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator-(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator*(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator/(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator!() const override { return std::make_unique<Universe>(); }

    std::unique_ptr<ISet> lt(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> gt(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> lteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> gteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }

    std::unique_ptr<ISet> equals(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> noteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> contains(const ISet & /*set*/, ISet *boolSet) const override {
        return std::make_unique<Bool>(false, boolSet);
    }

    std::unique_ptr<ISet> clone() const override { return std::make_unique<Ref>(this); }
    std::string show() const override { return "void set"; }
};

class Sets : public ISet, public std::map<std::string, std::unique_ptr<ISet>> {
public:
    Sets(node::Module const *module = nullptr, ISet *superset = nullptr) : _superset(superset), module(module) {}

    ISet const *thisset() const override { return this; }
    ISet const *superset() const override { return _superset; }

    std::unique_ptr<ISet> operator+(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator-(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator*(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator/(ISet const & /*set*/) const override { return nullptr; }
    std::unique_ptr<ISet> operator!() const override { return nullptr; }

    std::unique_ptr<ISet> lt(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> gt(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> lteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> gteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }

    std::unique_ptr<ISet> equals(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> noteq(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }
    std::unique_ptr<ISet> contains(ISet const & /*set*/, ISet * /*boolSet*/) const override { return nullptr; }

    std::unique_ptr<ISet> clone() const override { return std::make_unique<Ref>(this); }
    std::string show() const override { return "sets"; }

public:
    node::Module const *module;

private:
    ISet *_superset{};
};

struct Module {
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

inline std::unique_ptr<ISet> Identity::clone() const {
    return std::make_unique<Ref>(this);
}

inline std::unique_ptr<ISet> Universe::operator!() const {
    return std::make_unique<Void>();
}

} // namespace set
