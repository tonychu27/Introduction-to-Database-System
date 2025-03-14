SELECT order_id
FROM orders
WHERE order_delivered_carrier_date IS NOT NULL AND order_delivered_customer_date IS NOT NULL
ORDER BY DATEDIFF(order_delivered_customer_date, order_delivered_carrier_date) DESC
LIMIT 3;