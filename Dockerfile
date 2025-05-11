FROM alpine:latest

RUN apk add --no-cache build-base git pkgconf spdlog-dev bison flex openssl-dev
RUN git clone https://github.com/hyrise/sql-parser.git /tmp/sqlparser && cd /tmp/sqlparser && make && mkdir -p /usr/local/include/hsql && make install

WORKDIR /app

COPY . .

RUN make

CMD ["./beekDB"]
