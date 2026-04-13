module;

export module ctv.special_values;

export namespace ctv
{
    struct zero_type
    {
        template <typename T>
        constexpr operator T() const
        {
            return T{};
        }
    };

    inline constexpr zero_type zero{};

    struct one_type
    {
        template <typename T>
        constexpr operator T() const
        {
            return T{1};
        }
    };

    inline constexpr one_type one{};

    struct minus_one_type
    {
        template <typename T>
        constexpr operator T() const
        {
            return -T{1};
        }
    };

    inline constexpr minus_one_type minus_one{};
}
