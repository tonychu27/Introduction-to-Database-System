WITH PaymentCnt AS (
	SELECT payment_type, COUNT(*) AS payment_cnt
    FROM payments
    GROUP BY payment_type
    HAVING COUNT(*) > 100
),
TopTwo AS (
	SELECT payment_type
    FROM PaymentCNT
    ORDER BY payment_cnt DESC
    LIMIT 2
)
SELECT pc.payment_type, pc.payment_cnt AS cnt
FROM PaymentCnt pc
WHERE pc.payment_type NOT IN (SELECT payment_type FROM TopTwo)
ORDER BY pc.payment_cnt DESC;