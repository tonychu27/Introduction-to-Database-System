SELECT COUNT(DISTINCT o.order_id) AS cnt
FROM orders o
JOIN customers c ON c.customer_id = o.customer_id
JOIN payments p ON p.order_id = o.order_id
WHERE p.payment_type = 'debit_card' AND YEAR(o.order_purchase_timestamp) = 2017 AND c.customer_state = 'RJ';