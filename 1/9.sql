WITH filtered_items AS (
    SELECT seller_id, COUNT(*) AS cnt
    FROM order_items
    WHERE price > 150
    GROUP BY seller_id
    HAVING COUNT(*) > 100
)
SELECT seller_id, cnt
FROM filtered_items
ORDER BY cnt DESC
LIMIT 3;