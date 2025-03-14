SELECT c.customer_state AS state, COUNT(o.order_id) AS cnt 
FROM orders o
JOIN customers c ON o.customer_id = c.customer_id
WHERE order_status != 'delivered'
GROUP BY c.customer_state;