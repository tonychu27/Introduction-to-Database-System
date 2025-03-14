SELECT customer_city AS city
FROM customers
WHERE customer_state = 'RJ'
GROUP BY city
ORDER BY COUNT(*) DESC
LIMIT 3;