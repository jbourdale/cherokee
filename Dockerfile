FROM python:3.7-buster

WORKDIR /cherokee

RUN apt update
RUN apt install -y make build-essential python3-dev

COPY . .
COPY ./rootpath /tmp/cherokee
RUN make clean
RUN make

CMD /cherokee/Cherokee