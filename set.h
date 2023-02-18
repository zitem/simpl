#pragma once

namespace set {

struct Interface;
using $ = std::unique_ptr<Interface>;

template <typename T> concept derived = (std::derived_from<T, Interface> && !std::is_same_v<T, Interface>);

struct Bool;
class Set;
class Failure;

template <typename S = Failure, typename... Args> static Set create(Args &&...args);

struct Interface {
    virtual ~Interface() = default;

    virtual Interface const &thisset() const = 0;
    virtual Interface const &superset() const = 0;

    virtual $ operator==(Interface const &set) const = 0;
    virtual $ operator!=(Interface const &set) const = 0;
    virtual $ contains(Interface const &set) const = 0;
    virtual bool ok() const = 0;
    virtual bool solved() const { return true; }

    virtual $ extract(std::string_view name) const = 0;

    virtual $ clone() const = 0;
    virtual std::string show() const = 0;

    virtual $ resolve(Interface const &set) const;

    template <typename T> requires derived<T> T &cast() { return *static_cast<T *>(this); }
    template <typename T> requires derived<T> T const &cast() const { return *static_cast<T const *>(this); }
};

class Set {
public:
    Set($ &&set) : _set(std::move(set)) {}

    template <typename T> requires derived<T> T const &cast() const { return _set->cast<T>(); }
    template <typename T> requires derived<T> T &cast() { return _set->cast<T>(); }

    template <typename T> requires derived<T> std::unique_ptr<T> move();
    $ move();

    Set operator!=(Set const &set) const { return *_set != *set._set; }
    Set contains(Set const &set) const { return _set->contains(*set._set); }
    bool ok() const { return _set->ok(); }
    bool solved() const { return _set->solved(); }
    Set extract(std::string_view name) const { return _set->extract(name); }
    Set resolve(Set const &set) const { return _set->resolve(*set._set); }
    Set clone() const { return _set->clone(); }
    std::string show() const { return _set->show(); }

private:
    $ _set;
};

struct Identity : Interface {
    Identity() = default;

    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override;

    $ operator==(Interface const &set) const override;
    $ operator!=(Interface const &set) const override;
    $ contains(Interface const &set) const override { return thisset() == set.superset(); }
    bool ok() const override { return true; }
    $ extract(std::string_view name) const override;
    $ resolve(const Interface &set) const override;

    $ clone() const override;

    std::string show() const override { return std::format("id: {}", (void *)this); }
};

struct Failure : Interface {
    constexpr static Identity const super{};
    Failure(std::string msg = "failure") : msg(std::move(msg)) {}
    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override { return super; }

    $ operator==(Interface const & /*set*/) const override { return clone(); }
    $ operator!=(Interface const & /*set*/) const override { return clone(); }
    $ contains(Interface const & /*set*/) const override { return clone(); }
    bool ok() const override { return false; }
    $ extract(std::string_view /*name*/) const override { return clone(); }

    $ clone() const override { return std::make_unique<Failure>(msg); }
    std::string show() const override { return msg; }

    std::string msg;
};

class Unsolved : public Interface {
public:
    Unsolved(Interface const &super) : _super(super.clone()) {}
    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override { return *_super; }

    $ operator==(Interface const & /*set*/) const override { return std::make_unique<Failure>(); }
    $ operator!=(Interface const & /*set*/) const override { return std::make_unique<Failure>(); }
    $ contains(Interface const & /*set*/) const override { return std::make_unique<Failure>(); }
    bool ok() const override { return true; }
    bool solved() const override { return false; }

    $ extract(std::string_view /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Failure>(); }
    std::string show() const override { return "unsolved" + _super->show(); }
private:
    std::unique_ptr<Interface> _super;
};

struct Void : Interface {
    constexpr static Identity const id{};
    Interface const &thisset() const override { return id; } // ref of identity
    Interface const &superset() const override;

    $ operator==(Interface const &set) const override { return id == set; }
    $ operator!=(Interface const &set) const override { return id != set; }
    $ contains(Interface const &set) const override { return id == set; }
    bool ok() const override { return true; }
    $ extract(std::string_view /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Void>(); }
    std::string show() const override { return "void"; }
};

struct Universe : Interface {
    constexpr static Identity const id{};
    Interface const &thisset() const override { return id; }     // ref of identity
    Interface const &superset() const override { return *this; } // !

    $ operator==(Interface const &set) const override { return id == set; }
    $ operator!=(Interface const &set) const override { return id != set; }
    $ contains(Interface const & /*set*/) const override;
    bool ok() const override { return true; }
    $ extract(std::string_view /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Universe>(); }
    std::string show() const override { return "universe"; }
};

template <typename T> class Base : public Interface {
    constexpr static bool isBool = std::is_same_v<bool, T>;

public:
    constexpr static Identity const super{};

    Base(T val) : _val(val) {}

    Interface const &thisset() const override { return *this; }

    Interface const &superset() const override { return super; }

    $ operator==(Interface const &set) const override {
        return _binOp<[](auto lhs, auto rhs) { return lhs == rhs; }>(set);
    }

    $ operator!=(Interface const &set) const override {
        return _binOp<[](auto lhs, auto rhs) { return lhs != rhs; }>(set);
    }

    $ contains(Interface const &set) const override { return thisset() == set.superset(); }

    bool ok() const override { return true; }

    $ extract(std::string_view /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Base<T>>(_val); }

    std::string show() const override { return std::format("{}", _val); }

    $ resolve(const Interface & /*set*/) const override { return clone(); }

    T value() const { return _val; }

private:
    template <auto BinOp> $ _binOp(Interface const &set) const {
        if (&superset() != &set.superset()) return std::make_unique<Void>();
        auto res = BinOp(_val, set.thisset().cast<Base<T>>()._val);
        return std::make_unique<Base<decltype(res)>>(res);
    }

private:
    T _val;
};

struct IBool {
    virtual ~IBool() = default;
    virtual std::unique_ptr<Bool> boolean() const = 0;
    operator bool() const;
    using Fun = std::unique_ptr<Bool>(IBool const &rhs) const;
    Fun operator&, operator&&, operator|, operator||;
    std::unique_ptr<Bool> operator!() const;
};

template <typename T> struct ICalc {
    virtual ~ICalc() = default;
    using Fun = std::unique_ptr<T>(T const &rhs) const;
    virtual Fun operator+ = 0, operator- = 0, operator* = 0, operator/ = 0;
};

template <typename T> struct IEq {
    virtual ~IEq() = default;
    using Fun = std::unique_ptr<Bool>(T const &rhs) const;
    virtual Fun operator== = 0, operator!= = 0;
};

template <typename T> struct IOrd {
    virtual ~IOrd() = default;
    using Fun = std::unique_ptr<Bool>(T const &rhs) const;
    virtual Fun operator</**/ = 0, operator> = 0;
};

template <typename T> struct ICmp : IEq<T>, IOrd<T> {
    using Fun = std::unique_ptr<Bool>(T const &rhs) const;
    Fun operator<=, operator>=;
};

struct Bool final : Base<bool>, IBool {
    using Base::Base;
    std::unique_ptr<Bool> boolean() const override;
};

struct Int final : Base<int>, ICalc<Int>, ICmp<Int> {
    using Base::Base;
    ICalc::Fun operator+ override;
    ICalc::Fun operator- override;
    ICalc::Fun operator* override;
    ICalc::Fun operator/ override;
    IEq::Fun operator== override;
    IEq::Fun operator!= override;
    IOrd::Fun operator<override;
    IOrd::Fun operator> override;
    using ICmp::operator<=;
    using ICmp::operator>=;
    std::unique_ptr<Int> operator-() const;
};

class Ref : public Interface {
public:
    Ref(Interface const &set) : _set(set.thisset()) {}
    Interface const &thisset() const override { return _set; }
    Interface const &superset() const override { return _set.superset(); }

    $ operator==(Interface const &set) const override { return _set == set; }
    $ operator!=(Interface const &set) const override { return _set != set; }
    $ contains(Interface const &set) const override { return _set.contains(set); }
    bool ok() const override { return true; }
    $ extract(std::string_view name) const override { return _set.extract(name); }
    $ resolve(const Interface &set) const override { return _set.resolve(set); }

    $ clone() const override { return std::make_unique<Ref>(_set.thisset()); }
    std::string show() const override { return _set.show(); }

private:
    Interface const &_set;
};

class Sets : public Interface {
public:
    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override { return Universe::id; }

    $ operator==(Interface const &set) const override {
        if (&superset() != &set.superset()) {
            return std::make_unique<Bool>(false);
        }
        $ res = std::make_unique<Bool>(true);
        for (auto const &[k, v] : set.thisset().cast<Sets>()._data) {
            res = res->cast<Bool>() && (*_data.at(k) == *v)->cast<Bool>();
        }
        return res;
    }
    $ operator!=(Interface const & /*set*/) const override { return nullptr; }
    $ contains(Interface const & /*set*/) const override { return nullptr; }
    bool ok() const override {
        return std::ranges::all_of(_data, [](auto const &pair) { return pair.second->ok(); });
    }

    $ extract(std::string_view name) const override;
    $ resolve(Interface const &set) const override { return (void)set, nullptr; }
    bool add(std::string_view name, $ &&set) {
        if (!set->ok()) return true;
        auto find = _data.find(name);
        if (find != _data.end()) return false;
        _data.insert(find, {name, std::move(set)});
        return true;
    }

    $ clone() const override {
        auto res = std::make_unique<Sets>();
        for (auto const &[k, v] : _data) {
            res->cast<Sets>()._data[k] = v->clone();
        }
        return res;
    }
    std::string show() const override { return "sets"; }

private:
    std::map<std::string_view, $> _data;
};

template <typename Set, auto Impl> class Unary : public Interface {
public:
    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override { return Universe::id; }

    $ operator==(Interface const & /*set*/) const override { return std::make_unique<Failure>(); }
    $ operator!=(Interface const & /*set*/) const override { return std::make_unique<Failure>(); }
    $ contains(const Interface & /*set*/) const override { return std::make_unique<Failure>(); }
    bool ok() const override { return true; }
    $ extract(std::string_view /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Ref>(*this); }
    std::string show() const override { return "module"; }

    $ resolve(const Interface &set) const override {
        auto v = set.extract("v");
        if (v->superset().resolve(Void::id)->operator!=(Set::super)->template cast<Bool>().value()) {
            return std::make_unique<Failure>();
        }
        auto val = v->resolve(Void::id);
        if (!val->ok()) {
            return std::make_unique<Unsolved>(Set::super);
        }
        auto extract = Impl(val->thisset().cast<Set>());
        auto sets = std::make_unique<Sets>();
        sets->add("extract", std::move(extract));
        return sets;
    }
};

template <typename Set, auto Impl> class Binary : public Interface {
public:
    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override { return Universe::id; }

    $ operator==(Interface const & /*set*/) const override { return std::make_unique<Failure>(); }
    $ operator!=(Interface const & /*set*/) const override { return std::make_unique<Failure>(); }
    $ contains(const Interface & /*set*/) const override { return std::make_unique<Failure>(); }
    bool ok() const override { return true; }
    $ extract(std::string_view /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Ref>(*this); }
    std::string show() const override { return "module"; }

    $ resolve(const Interface &set) const override {
        auto x = set.extract("x");
        auto y = set.extract("y");
        auto superOk = []($ const &s) {
            auto const &super = s->superset();
            auto resolve = super.resolve(Void::id);
            auto eq = resolve->operator==(Set::super);
            auto cast = eq->template cast<Bool>();
            return cast.value();
        };
        if (!superOk(x) || !superOk(y)) {
            return std::make_unique<Failure>();
        }
        auto xx = x->resolve(Void::id);
        auto yy = y->resolve(Void::id);
        if (!xx->ok() || !yy->ok()) {
            return std::make_unique<Unsolved>(Set::super);
        }
        auto extract = Impl(xx->thisset().cast<Set>(), yy->thisset().cast<Set>());
        auto sets = std::make_unique<Sets>();
        sets->add("extract", std::move(extract));
        return sets;
    }
};

class Array : public Interface {
public:
    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override { return Universe::id; }

    $ operator==(Interface const &set) const override {
        if (&superset() != &set.superset()) {
            return std::make_unique<Bool>(false);
        }
        $ res = std::make_unique<Bool>(true);
        auto const &array = set.thisset().cast<Array>()._data;
        for (auto i = 0; i < array.size(); ++i) {
            res = res->cast<Bool>() && (*_data[i] == *array[i])->cast<Bool>();
        }
        return res;
    }
    $ operator!=(Interface const & /*set*/) const override { return nullptr; }
    $ contains(Interface const & /*set*/) const override { return nullptr; }
    bool ok() const override {
        return std::ranges::all_of(_data, [](auto const &ele) { return ele->ok(); });
    }

    void append($ &&set) { _data.push_back(std::move(set)); }
    void set(size_t idx, $ &&set) { _data[idx] = std::move(set); }
    void resize(size_t size) { _data.resize(size); }

    $ extract(std::string_view /*name*/) const override { return nullptr; }
    $ clone() const override { return std::make_unique<Ref>(thisset()); }
    std::string show() const override { return "array"; }
private:
    std::vector<$> _data;
};

using Not = Unary<Bool, [](auto v) { return !v; }>;
using Neg = Unary<Int, [](auto v) { return -v; }>;
using Add = Binary<Int, [](auto x, auto y) { return x + y; }>;
using Sub = Binary<Int, [](auto x, auto y) { return x - y; }>;
using Mul = Binary<Int, [](auto x, auto y) { return x * y; }>;
using Div = Binary<Int, [](auto x, auto y) { return x / y; }>;
using Lt = Binary<Int, [](auto x, auto y) { return x < y; }>;
using Gt = Binary<Int, [](auto x, auto y) { return x > y; }>;
using Lteq = Binary<Int, [](auto x, auto y) { return x <= y; }>;
using Gteq = Binary<Int, [](auto x, auto y) { return x >= y; }>;
using And = Binary<Bool, [](auto x, auto y) { return x && y; }>;
using Or = Binary<Bool, [](auto x, auto y) { return x || y; }>;
template <typename T> using Eq = Binary<T, [](auto x, auto y) { return x.operator==(y); }>;
template <typename T> using Noteq = Binary<T, [](auto x, auto y) { return x.operator!=(y); }>;

// // // // // //
//  implement  //
// // // // // //

template <typename S, typename... Args> static Set create(Args &&...args) {
    return Set{std::make_unique<S>(std::forward<Args>(args)...)};
}

inline $ Interface::resolve(const Interface & /*set*/) const {
    return std::make_unique<Failure>();
}

template <typename T> std::unique_ptr<Bool> ICmp<T>::operator<=(T const &rhs) const {
    return *(*this < rhs) || *(*this == rhs);
}

template <typename T> std::unique_ptr<Bool> ICmp<T>::operator>=(T const &rhs) const {
    return *(*this < rhs) || *(*this == rhs);
}

// Set
template <typename T> requires derived<T> std::unique_ptr<T> Set::move() {
    auto out = std::make_unique<T>(std::move(_set.release()->cast<T>()));
    _set = std::make_unique<Failure>();
    return out;
}

inline $ Set::move() {
    auto out = std::move(_set);
    _set = std::make_unique<Failure>();
    return out;
}

// Identity
inline Interface const &Identity::superset() const {
    return Universe::id;
}

inline $ Identity::operator!=(Interface const &set) const {
    return std::make_unique<Bool>(this != &set.thisset());
}

inline $ Identity::operator==(Interface const &set) const {
    return std::make_unique<Bool>(this == &set.thisset());
}

inline $ Identity::extract(std::string_view /*name*/) const {
    return std::make_unique<Failure>();
}

inline $ Identity::clone() const {
    return std::make_unique<Ref>(*this);
}

inline $ Identity::resolve(const Interface & /*set*/) const {
    return std::make_unique<Ref>(*this);
}

// Void
inline Interface const &Void::superset() const {
    return Universe::id;
}

// Universe
inline $ Universe::contains(Interface const & /*set*/) const {
    return std::make_unique<Bool>(true);
}

// Sets
inline $ Sets::extract(std::string_view name) const {
    if (name.empty()) {
        auto res = std::make_unique<Sets>();
        for (auto const &[k, v] : _data) {
            if (v->ok()) {
                res->_data.insert({k, v->clone()});
            }
        }
        return res;
    }
    auto findset = _data.find(name);
    if (findset != _data.end()) {
        auto const &set = *findset->second;
        return set.clone();
    }
    return std::make_unique<Failure>();
}

// std
inline Set std() {
    auto sets = std::make_unique<Sets>();
    sets->add("bool", Bool::super.clone());
    sets->add("int", Int::super.clone());
    sets->add("Add", std::make_unique<Add>());
    sets->add("Sub", std::make_unique<Sub>());
    sets->add("Mul", std::make_unique<Mul>());
    sets->add("Div", std::make_unique<Div>());
    sets->add("Not", std::make_unique<Not>());
    sets->add("Neg", std::make_unique<Neg>());
    sets->add("Lt", std::make_unique<Lt>());
    sets->add("Gt", std::make_unique<Gt>());
    sets->add("Lteq", std::make_unique<Lteq>());
    sets->add("Gteq", std::make_unique<Gteq>());
    sets->add("And", std::make_unique<And>());
    sets->add("Or", std::make_unique<Or>());
    sets->add("Eq_Int", std::make_unique<Eq<Int>>());
    sets->add("Eq_Bool", std::make_unique<Eq<Bool>>());
    sets->add("Noteq_Int", std::make_unique<Noteq<Int>>());
    sets->add("Noteq_Bool", std::make_unique<Noteq<Bool>>());
    return {std::move(sets)};
}

} // namespace set
