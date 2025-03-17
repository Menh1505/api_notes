# Danh sách nâng cấp API

## Cải tiến cơ bản

### 1. Cải thiện quản lý dữ liệu

- [ ] 1.1. Dùng std::unordered_map<int, note> thay vì std::vector<note>
  - Lý do: Việc lưu trữ dữ liệu bằng std::vector có thể gây ra lỗi khi xóa ghi chú (do chỉ số bị thay đổi).
  - std::unordered_map giúp tìm kiếm nhanh hơn và không ảnh hưởng đến ID khi xóa.
- [ ] 1.2. Cải thiện chức năng DELETE để tránh lỗi khi xóa phần tử
  - Khi xóa bằng vector.erase(), tất cả các phần tử phía sau bị dịch chuyển, gây lỗi nếu truy cập bằng ID.
  - Dùng std::unordered_map<int, note> sẽ giúp khắc phục lỗi này.

### 2. Kiểm tra dữ liệu đầu vào

- [ ] 2.1. Kiểm tra dữ liệu JSON hợp lệ trước khi tạo hoặc cập nhật ghi chú
  - Hiện tại, nếu JSON không có đủ trường, chương trình có thể bị lỗi runtime. Hãy kiểm tra JSON kỹ trước khi truy cập vào các key.

### 3. Trả về phản hồi chi tiết hơn

- [ ] Hiện tại, API chỉ trả về mã trạng thái (200, 201, 400), nhưng không có thông báo cụ thể. Hãy thêm thông báo rõ ràng để giúp debug dễ dàng hơn.

### 4. Cải thiện cấu trúc mã

- [ ] Tách API vào một class NoteAPI để dễ mở rộng
- [ ] Dùng std::mutex để tránh điều kiện race khi truy cập notes trong môi trường đa luồng

### 5. Hỗ trợ ID tự động tăng

- [ ] Tránh dùng notes.size() làm ID vì khi xóa, ID có thể bị trùng lặp. Dùng biến next_id để sinh ID duy nhất.

### 6. Trả về toàn bộ ghi chú dưới dạng danh sách thay vì object JSON

### 7. Xử lý lỗi khi gọi API không hợp lệ

- [ ] Nếu ID không tồn tại, trả về lỗi 404
- [ ] Nếu dữ liệu JSON sai định dạng, trả về lỗi 400

### 8. Hỗ trợ API pagination (phân trang)

- [ ] Khi dữ liệu quá lớn, hỗ trợ phân trang giúp giảm tải

## Cải tiến nâng cao

### 1. Tách API thành một class NoteAPI

**Mã hiện tại viết tất cả logic trong main.cpp, gây khó bảo trì.**

- [ ] Cải tiến: Tách API thành class để dễ quản lý và mở rộng.

### 2. Lưu dữ liệu vào database (SQLite hoặc PostgreSQL)

**Hiện tại API lưu dữ liệu trong RAM, mất toàn bộ dữ liệu khi tắt server.**

- [ ] Cải tiến: Dùng SQLite/PostgreSQL thay vì std::unordered_map.
      Thư viện gợi ý: - SQLite: [sqlite_modern_cpp]() - PostgreSQL: [libpqxx]()

### 3. Thêm xác thực người dùng (JWT hoặc API Key)

**API hiện tại ai cũng có thể gọi, dễ bị tấn công.**

- [ ] Cải tiến: Thêm xác thực bằng JWT hoặc API Key.
      Thư viện gợi ý: jwt-cpp

### 4. Cấu hình CORS cho API

**Nếu muốn dùng API từ frontend, cần hỗ trợ CORS.**

- [ ] Cải tiến: Dùng middleware CORS.

### 5. Thêm logging chi tiết

**Ghi log giúp debug lỗi production dễ dàng hơn.**

- [ ] Thư viện gợi ý: spdlog

## Nâng cấp production-ready

### 1. Load Balancing (Cân bằng tải)

- Khi API mở rộng, cần nhiều instance chạy song song để tránh quá tải.
- Giải pháp: Dùng Nginx, HAProxy, hoặc Traefik để phân phối request.
- Cấu hình Nginx Load Balancer (ví dụ chạy 3 instances API trên cổng 3001, 3002, 3003):

```nginx
upstream api_servers {
    server 127.0.0.1:3001;
    server 127.0.0.1:3002;
    server 127.0.0.1:3003;
}

server {
    listen 80;
    location / {
        proxy_pass http://api_servers;
    }
}
```

- Tích hợp Docker + Kubernetes nếu muốn mở rộng linh hoạt.

### 2. Caching với Redis

- Vấn đề: Mỗi lần gọi API đều truy vấn database → tốn tài nguyên.
- Giải pháp: Lưu trữ các truy vấn phổ biến trong Redis để tăng tốc độ.
- Thực hiện:
  - Dùng thư viện hiredis hoặc redis-plus-plus cho C++.
  - Khi truy vấn dữ liệu:
    1. Kiểm tra Redis trước.
    2. Nếu không có, lấy từ database và lưu vào Redis.
- Ví dụ:

```cpp
#include <sw/redis++/redis++.h>
using namespace sw::redis;

Redis redis("tcp://127.0.0.1:6379");

std::optional<std::string> getCachedNote(int id) {
    return redis.get("note:" + std::to_string(id));
}

void cacheNote(int id, const std::string &data) {
    redis.setex("note:" + std::to_string(id), 3600, data); // Lưu cache trong 1 giờ
}
```

### 3. Database Scaling (Sharding & Replication)

- Vấn đề: Khi dữ liệu quá lớn, 1 database server không đủ.
- Giải pháp:
  - Master-Slave Replication (1 database chính, nhiều bản sao chỉ đọc).
  - Sharding: Chia dữ liệu thành nhiều database nhỏ theo ID.
- Ví dụ (PostgreSQL Replication):
  - Master:

```sql
CREATE USER replica WITH REPLICATION ENCRYPTED PASSWORD 'password';
```

    - Slave:

```sh
pg_basebackup -h master_ip -D /var/lib/postgresql/data -U replica -P -R
```

### 4. Rate Limiting (Chống DDoS)

- Vấn đề: Nếu có quá nhiều request từ một client, server có thể bị sập.
- Giải pháp: Dùng Token Bucket Algorithm hoặc Redis-based Rate Limiting.
- Ví dụ (Giới hạn mỗi IP chỉ gọi API 100 lần/phút):

```cpp
bool isRateLimited(const std::string &ip) {
    int requests = std::stoi(redis.get("rate:" + ip).value_or("0"));
    if (requests >= 100) return true;
    redis.incr("rate:" + ip);
    redis.expire("rate:" + ip, 60);
    return false;
}
```

### 5. Authentication & Authorization

#### 5.1. Xác thực bằng JWT

- Giải pháp: Dùng JSON Web Token (JWT) để xác thực.
- Ví dụ (Dùng jwt-cpp):

```cpp
auto token = jwt::create()
    .set_issuer("MyAPI")
    .set_type("JWT")
    .set_payload_claim("user", jwt::claim("admin"))
    .sign(jwt::algorithm::hs256{"secret"});
```

Khi nhận request

```cpp
auto decoded = jwt::decode(token);
if (decoded.has_issuer("MyAPI")) {
    // Token hợp lệ
}
```

#### 5.2. OAuth 2.0 (Google, Facebook Login)

- [ ] Tích hợp OAuth với Google để user có thể đăng nhập.
- [ ] Dùng thư viện oatpp để kết nối OAuth 2.0.

### 6. HTTPS & Bảo mật API

- Dùng HTTPS với Let's Encrypt:

```sh
certbot --nginx -d example.com
```

- Thêm HTTP Headers bảo mật:

```nginx
add_header X-Frame-Options DENY;
add_header X-Content-Type-Options nosniff;
add_header X-XSS-Protection "1; mode=block";
```

### 7. Containerization với Docker & Kubernetes

- Đóng gói API vào Docker để dễ triển khai.
- Triển khai Kubernetes (K8s) để scale lên hàng ngàn instance.
- Ví dụ Dockerfile:

```dockerfile
FROM ubuntu:20.04
RUN apt update && apt install -y crow clang libssl-dev
COPY . /app
WORKDIR /app
CMD ["./my_api"]
```

- Triển khai Kubernetes với Helm Chart.
