# imx6ull-ecspi

This library API provides direct access to i.MX 6ULL ECSPI hardware. Currently maximum burst length is 256 bytes and Slave Select is asserted for the whole transfer.

## Initialization and configuration

To initialize one of four ECSPI instances use
```c
int ecspi_init(int dev_no, uint8_t chan_msk);
```
where `dev_no` ∊ {1, 2, 3, 4} indicates which instance to initialize, and `chan_msk` is a 4-bit bitmask stating which Slave Select lines (channels) for a given instance will be enabled. I/O multiplexers and daisies for ECSPI pins are implicitly set-up to correct values — but only for the enabled instance and enabled channels.

Instead of specifying numbers for `dev_no` one could use predefined enums: `ecspi1`, `ecspi2`, `ecspi3`, `ecspi4` which correspond to 1, 2, 3, and 4 respectively.

Default configuration set after calling this procedure is as follows: SPI mode 0, channel 0, all channels as masters, no clock division, and no CS-to-SCLK delay. And thus default clock is 60 MHz.


To set the current channel use
```c
int ecspi_setChannel(int dev_no, uint8_t chan);
```
where `chan` ∊ {0, 1, 2, 3} indicates a channel to use. Any subsequent exchange operation will use this channel. Only enabled channel can be selected — doing otherwise will abort with an error value.


To set the current mode of SPI operation use
```c
int ecspi_setMode(int dev_no, uint8_t chan, uint8_t mode);
```
where `chan` is a channel to configure (no need to set the current one beforehand), and `mode` ∊ {0, 1, 2, 3} is a (CPOL, CPHA) value to use.


To set the clock dividers use
```c
int ecspi_setClockDiv(int dev_no, uint8_t pre, uint8_t post);
```
where `pre` ∊ \[0x0, 0xF\] is a pre-divider value and `post` ∊ \[0x0, 0xF\] is a post-divider value for the clock signal supplied to ECSPI instance. The values' interpretation can be found in i.MX reference manual.


To set a delay between asserting CS (Chip Select, Slave Select) and the first SPI clock edge (a CS-to-SCLK delay) use
```c
int ecspi_setCSDelay(int dev_no, uint8_t delay);
```
where `delay` ∊ \[0, 63\] is how many SPI clocks shall be inserted.

### Example

```c
/* Enable 4th instance, and channels 0 and 1. */
ecspi_init(ecspi4, 0x03);

ecspi_setMode(ecspi4, 1, 2);
ecspi_setMode(ecspi4, 0, 0);

ecspi_setChannel(ecspi4, 1);
/* Set clock to ~114 Hz, assuming clock root was 60 MHz. */
ecspi_setClockDiv(ecspi4, 0xF, 0xF);
/* Set CS-to-SCLK delay to ~87 ms. */
ecspi_setCSDelay(ecspi4, 10);
```

## Data exchange

Data read and write proceed simultaneously so only one procedure is used for data transmission:
```c
int ecspi_exchange(int dev_no, const uint8_t *out, uint8_t *in, uint16_t len);
```
where `out` is a pointer to outgoing data, `in` is a pointer for incoming data, and `len` is the length of `out` and also `in`.

### Example

```c
uint8_t data[16] = {0x66, 0x12, 0};

/* Data in `out` are written to ECSPI buffer before writing incoming ones to `in` so passing the same pointer works. */
ecspi_exchange(ecspi4, data, data, 16);
```
