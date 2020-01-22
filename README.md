# WebHelper

Yet another library for HTTP requests.

## That's nice, but why?

This is part of a project that I've been working on for the past 2 days or so, but because of some complications, I can no longer continue with. I'm well aware that I'm reinventing the wheel here, and worse still, I'm almost certainly not bringing anything new to the table. However, this whole thing was just a distraction, I just wanted to see how far I could go without needing any external libraries and without spending hundreds of hours. And the answer is (unsurprisingly) not that far.

Anyways, if for whatever reason you do decide to use this, just keep in mind my knowledge about HTTP is about as extense as the first 100 lines of so of its Wikipedia page. **WebHelper** ***is*** capable of retrieving HTML code from an **HTTP** (it doesn't support HTTPS) server, but I realy didn't test it's other features that much.

## Usage

Take a look at the examples too see a better explanation on how to use it.

## Installation

```
make
sudo make install
```

You may need to also create a link to `/usr/lib/` and reload `ldconfig`:

```
sudo ln -s /usr/local/lib/libwebhelper.* /usr/lib/
sudo ldconfig -v
```