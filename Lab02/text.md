Colaborative filterling

It would be much simpler if we used algorithms supported by other libraries. However, the main goal of this lab is to help you practice using the Numpy library. Therefore, you will have the opportunity to build a simple anime recommendation system from scratch using the provided data, utilizing only Numpy library. Please remember that Numpy doesn't favor loops, so you are only allowed to use loops where I explicitly permit.

In my opinion, there are two fundamental tasks in a anime recommendation system:

- First, you need to predict the ratings for animes that a user hasn't reviewed or watched yet.
- Second, you need to provide recommendations to users based on the top-rated animes that have been predicted.

It seems that the second task will become much simpler if we can accomplish the first task. One of the simplest ways to tackle task 1 is by computing the similarity between users and using this similarity to make predictions. However, there are some considerations to keep in mind. It's not feasible to compute similarity between all users at once, as it might lead to memory issues (even if you have enough memory, my computer is quite limited in that regard :<). One way to address this issue is to process a group of users at a time, referred to as `a batch`. To keep it simple, let's stick with a `batch_size = 32`, which I believe is a reasonable value. You should try to make your code work with a single batch first and then extend it to process all batches.
```python
batch_size = 32
start = 0
end = batch_size
```
Step 1: Calculate the `similarities` array to show the similarity between each user in the current batch with all users in the entire dataset. This array will have a size of `batch_size` x `n_users` (`n_users` is the total number of users in the dataset), where `similarities[i, j]` indicates the similarity between `user_i` and `user_j`. In the case where two users have no common rated movies (when running, you may see a warning 'RuntimeWarning: Mean of empty slice'), you set the similarity to 0.
```python
# YOUR CODE HERE
# Mở rộng ratings_mat thành một tensor 3D, sao cho bạn có thể trừ mỗi hàng với mọi hàng khác.
batch_mat = ratings_mat[:batch_size]
batch_expanded = np.expand_dims(batch_mat, axis=1) # (32, 1, 2000)

ratings_expanded = np.expand_dims(ratings_mat, axis = 0) # (1,1001,2000)

# Broadcast để trừ mỗi hàng với mọi hàng khác
diff_mat = np.abs( batch_expanded- ratings_expanded ) #(32, 1001, 2000)

mean_mat = np.nanmean(diff_mat, axis=-1) #(32, 1001)
# Tạo mảng similarites
similarities = 1 / (mean_mat + 0.001)
```
Step 2: calculate the `weights` matrix. The array `weights` will have the size `batch_size` x `n_users` x `n_movies` (where `n_movies` is the total number of movies). About how to calculate `weights`, you can refer to file `example.ipynb`.

When running, you will see the warning "RuntimeWarning: invalid value encountered in true_divide"; This is because the users who rate a movie under consideration all have a similarity of 0 with a user under review, resulting in normalization to 0/0 and the result is difficult. This case means there is not enough information to predict the score and in this article, you should leave it as it is.

```python
weights = np.zeros((batch_size, number_user, number_anime))

for i in range(batch_size):
    r = ratings_mat[i]  
    
    a = ratings_mat[:, np.isnan(r)] #(1001,...)
    
    w = ~np.isnan(a) * similarities[i].reshape(-1, 1)#(1001,...)
    w = w/ (w.sum(axis=0))
    weights[i, :, np.isnan(r)] = w.T
print(weights.shape)
print(np.sum(np.isnan(weights)))
```

Step 3: For each user in the batch under consideration, calculate the score (for all movies) by multiplying the score of all users with the corresponding weight in the `weight` array; then write each user's scores down to one line in the `filled_ratings` array. 
```python
r0 = ratings_mat[0]
nan_indices = np.isnan(ratings_mat)

w0 = weights[0,:,nan_indices].T
a = ratings_mat[:, nan_indices]

filled_ratings[0, nan_indices] = np.nansum(w0 * a, axis=0)
```









Example
```python
ratings = np.array([[5, 4, np.nan, 1, np.nan], [np.nan, 4, 4, 2, 2],
                    [3, np.nan, np.nan, 5, 5],[np.nan, 3, 4, 4, np.nan]])
ratings

np.set_printoptions(suppress = True)

r0 = ratings[0]
# Cách lấy similarity
# Lấy tất cả các user trừ cho user đầu tiên để tính khoảng cách, 
# sau đó tính mean của từng dòng để thu được độ tương tự giữa các user đối với user[0]
s = np.nanmean(np.abs(ratings - r0), axis = 1)

s = 1/(s + 0.001)
s
# Cách lấy weight
# Lấy ra các cột mà user[0] chưa đánh giá để 
a = ratings[:, np.isnan(r0)]
a
# Tạo ma trận trọng số dựa trên độ tương tự
w = ~np.isnan(a)*s.reshape(-1, 1)
print(w)
# Chuẩn hoá trọng số
print(w.sum(axis =0))
w = w/w.sum(axis = 0)
w
```
Tôi đã tổng quát hóa lên và xây dựng 1 ma trận similarities có kích thước là batch x n_user
```python
batch_size = 3
batch_mat = ratings[:batch_size]
batch_expanded = np.expand_dims(batch_mat, axis=1) # (32, 1, 2000)

ratings_expanded = np.expand_dims(ratings_mat, axis = 0) # (1,1001,2000)

# Broadcast để trừ mỗi hàng với mọi hàng khác
diff_mat = np.abs( batch_expanded- ratings_expanded ) #(32, 1001, 2000)

mean_mat = np.nanmean(diff_mat, axis=-1) #(32, 1001)
# Tạo mảng similarites
similarities = 1 / (mean_mat + 0.001)

```
vì vậy tôi muốn xây dựng tensor 3D weights có kích thước batch_size * number_user * number_movies

```python

# # Tạo mask cho các đánh giá còn thiếu
# mask = np.isnan(batch_mat)[:, np.newaxis, :] & ~np.isnan(ratings_mat)

# # Mở rộng similarities để phù hợp với kích thước của mask
# similarities_expanded = similarities[:, :, np.newaxis]

# # Áp dụng similarities vào mask để tạo weights
# weights = mask * similarities_expanded

# # Chuẩn hoá weights theo trục người dùng
# weights_sum = np.sum(weights, axis=1, keepdims=True)
# weights_normalized = weights / (weights_sum + 0.001) # Thêm một giá trị nhỏ để tránh chia cho 0

# # Kiểm tra kích thước của weights_normalized để đảm bảo nó phù hợp
# print(weights_normalized.shape)

# print(np.sum(np.isnan(weights_normalized)))
weights = np.zeros((batch_size, number_user, number_anime))

for i in range(batch_size):
    r = ratings_mat[i]  
    
    a = ratings_mat[:, np.isnan(r)] #(1001,...)
    
    w = ~np.isnan(a) * similarities[i].reshape(-1, 1)#(1001,...)
    w = w/ (w.sum(axis=0))
    weights[i, :, np.isnan(r)] = w.T
print(weights.shape)
print(np.sum(np.isnan(weights)))
```

```python
weights = np.zeros((batch_size, number_user, number_anime))

a = ratings_mat[:, np.newaxis, :]  # shape: (n_users, 1, n_movies)
a = np.repeat(a, batch_size, axis=1)  # shape: (n_users, batch_size, n_movies)
a = a.transpose((1, 0, 2))  # shape: (batch_size, n_users, n_movies)

weights = ~np.isnan(a) * similarities[:batch_size, :, np.newaxis]  # shape: (batch_size, n_users, n_movies)
weights = weights / weights.sum(axis=1, keepdims=True)

```