SELECT c.customer_state AS state, o.order_id
FROM orders o
JOIN customers c ON o.customer_id = c.customer_id
JOIN (
	SELECT c2.customer_state, MAX(DATEDIFF(o2.order_delivered_customer_date, o2.order_delivered_carrier_date)) AS max_days
    FROM orders o2
    JOIN customers c2 ON o2.customer_id = c2.customer_id
    WHERE o2.order_status = 'delivered'
    GROUP BY c2.customer_state
) max_delivery ON c.customer_state = max_delivery.customer_state
AND DATEDIFF(o.order_delivered_customer_date, o.order_delivered_carrier_date) = max_delivery.max_days
WHERE order_status = 'delivered'
ORDER BY c.customer_state DESC;