
function is_prime(N)
    for x = 2, N^(1/2) do
        if N % x == 0 then
            return 0
        end
    return 1
    end
end
