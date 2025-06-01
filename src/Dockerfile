# syntax=docker/dockerfile:1

FROM alpine AS build
LABEL org.opencontainers.image.authors="sample name"
RUN apk add gcc libc-dev
COPY main.c /main.c
RUN mkdir rfs \
    && gcc -static main.c -o rfs/server
COPY rfs/* /rfs/

FROM scratch AS production
LABEL org.opencontainers.image.authors="sample name"
COPY --from=build rfs/* /
EXPOSE 8080
HEALTHCHECK --interval=30s --start-period=5s CMD ["/server", "--healthcheck"]
ENTRYPOINT [ "/server" ]
