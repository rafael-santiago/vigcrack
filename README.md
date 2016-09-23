# Vigcrack

*An old trinket from 2005/2003.*

``Vigcrack`` is a cryptoanalysis tool which can help on crack ``Vigenère`` ciphertexts. This tool is not strictly based on
``Babbage's``/``Kasiski's`` approach. In fact, here is not performed frequency analysis. Here what we get is a more brainy
``Hangman`` which does not hang any innocent stick figure...

## How to build it?

For building this software you need to use [Hefesto](https://github.com/rafael-santiago/hefesto). After following the
steps to get Hefesto installed on your system, you should move to the vigcrack's ``src`` sub-directory and call ``hefesto``:

```
babbage@DiffEngine:~/src/vigcrack# cd src
babbage@DiffEngine:~/src/vigcrack/src# hefesto
```

After invoking the hefesto command you should get the ``Vigcrack`` binary inside the sub-directory ``bin``.

## How to use it?

The ``Vigcrack`` is a software that introduces several operation modes. All of them related with the
cryptoanalysis of the ``Vigenère`` cipher.

In order to introduce you these operation modes I will use the basic workflow of it which is based on the necessity of
crack some encrypted data. So, let's use or good and old "crypto-actors": Alice, Bob and Eve.
Spoiler: today Eve will win! Come on, I think that she deserves, [look](https://xkcd.com/177).

Let's suppose that Bob sent to Alice the following message:

```
YLPHJVGIOVHWADRGVIEHRJPTUCYDSEIFMQFETUQKWEOGNVASCJVGKTCDEUZOWXEIVXOPMGVHKUKEVNFGRUAEJEHQNDKAMJUIIJTNFGWMLQPCQTQWODNGAR
VJBBIVJDQFSQEOWDTEBXUPUKPILBWPDUVAOWVYPXHCVEXLVUPUFQDQUXPFLNCNAVRAXHCVEXLVUPUFQDQUXPFLNCNAVRAMOQMFQYVAGLGETKVROJNAQUTM
EYFCCPIPLHAWOVKOPAMIFCCPTGYEOFOWVOPALADOTPETSSYLEFKNAVYNSOQOIPLZASBGNIGCIPIEOKNGCINBSUWMGZXQDKKPBGSMAGTJGRGPWWMIGRRQTM
OFIURRQTMOFAPGYGMSNBNGAEYLZAHOVUOOLXDJNIVOTLZABLPQOPLGWOKPQWJVAEUFGGLUDLWUEXGRAVYZPDQPTVLPHBNAQNGDLWUEXGRAVYZPDQPTVLPH
BNAQNGDLWUEXGRAVYZPDQPTVLPHBNAQNGDLWUEXGRAVYZPDQPTVLPHJTJKNMFSQBLTGAFFOJPWJQWHHVEEGQPOVASOBYAQUMUSSUHGCRVPWJUGQPEKTXWL
IPIMAZSJHTQVHGNVWWEYJAVLZASYQWDQKSJUTGNLCUCKOEYJAVLZASYQWDQKSJUTGNLCUCKOEYJAVLZASYQWDQKSJUTGNLCUCKOEYJAVLZASYQWDQKSJUT
GNL
```
## The Sybil

Eve is a "Network geek" so she has been listed the Alice/Bob's wire and intercepted the previous data. Eve knows that
Alice and Bob are pretty naive. So after trying to crack the cryptogram using simple substitutions without success, she
start assuming the ``Vigenère``cipher usage (due to the frequency distribution). By the way, Eve has installed a nice
[software](https://github.com/rafael-santiago/vigcrack) ;)... The first thing that our sympathetic attacker did with
this tool was query a smart Sybil:

```
eve@EDEN:~/src/tree/sin/curiosity# vigcrack --sybil \
> --file-path=alice-bob-crap.txt --min-pattern-len=3 --max-pattern-len=3
```

The ``Vigcrack's sybil`` returned the following supressed content to Eve:

```
INFO: 711 bytes loaded from alice-bob-crap.txt.

YLPHJVGIOVHWADRGVIEHRJPTUCYDSEIFMQFETUQKWEOGNVASCJVGKTCDEUZOWXEIVXOPMGVHKUKEVNFGRUAEJEHQNDKAMJUIIJTNFGWMLQPCQTQWODNGARVJBBIVJDQF
SQEOWDTEBXUPUKPILBWPDUVAOWVYPXHCVEXLVUPUFQDQUXPFLNCNAVRAXHCVEXLVUPUFQDQUXPFLNCNAVRAMOQMFQYVAGLGETKVROJNAQUTMEYFCCPIPLHAWOVKOPAMI
FCCPTGYEOFOWVOPALADOTPETSSYLEFKNAVYNSOQOIPLZASBGNIGCIPIEOKNGCINBSUWMGZXQDKKPBGSMAGTJGRGPWWMIGRRQTMOFIURRQTMOFAPGYGMSNBNGAEYLZAHO
VUOOLXDJNIVOTLZABLPQOPLGWOKPQWJVAEUFGGLUDLWUEXGRAVYZPDQPTVLPHBNAQNGDLWUEXGRAVYZPDQPTVLPHBNAQNGDLWUEXGRAVYZPDQPTVLPHBNAQNGDLWUEXG
RAVYZPDQPTVLPHJTJKNMFSQBLTGAFFOJPWJQWHHVEEGQPOVASOBYAQUMUSSUHGCRVPWJUGQPEKTXWLIPIMAZSJHTQVHGNVWWEYJAVLZASYQWDQKSJUTGNLCUCKOEYJAV
LZASYQWDQKSJUTGNLCUCKOEYJAVLZASYQWDQKSJUTGNLCUCKOEYJAVLZASYQWDQKSJUTGNL

INFO: Now guessing the key length... please wait...

*** Found patterns [nearest - farthest]:

	The pattern GCI was found 9 letters far from another GCI
	The pattern RRQ was found 9 letters far from another RRQ
	The pattern RQT was found 9 letters far from another RQT
	The pattern QTM was found 9 letters far from another QTM
	The pattern TMO was found 9 letters far from another TMO
	The pattern MOF was found 9 letters far from another MOF
	The pattern FCC was found 18 letters far from another FCC
	The pattern CCP was found 18 letters far from another CCP
	The pattern OPA was found 18 letters far from another OPA
	The pattern LZA was found 18 letters far from another LZA
	The pattern NFG was found 22 letters far from another NFG
	The pattern UPU was found 27 letters far from another UPU
	The pattern XHC was found 27 letters far from another XHC
(...)
	The pattern LPH was found 495 letters far from another LPH
	The pattern VAS was found 513 letters far from another VAS
	The pattern LPH was found 522 letters far from another LPH
	The pattern PHJ was found 522 letters far from another PHJ
	The pattern GNV was found 560 letters far from another GNV

*** Probable key lengths [most - least]: 3, 9, 2, 6, 18, 4, 5, 11, 12, 15, 8, 19, 7, 10, 13, 16, 20, 14.
```

>Why Eve ran that previous command? Why that pattern lengths?

Well, ``Vigcrack`` can predict the length of the keyword just verifying some characteristics onto the cryptogram. Its
``Sybil`` tell us at the end of the analysis report the most probable keyword lengths. The reason for
using the options ``--min-pattern-len=3`` and ``--max-pattern-len=3`` is because Eve is smart. The keyword guessing is
done by found patterns along the cryptogram. Eve supposed that the plaintext was in English and she knows that trigrams
are pretty common in a English text. Things like ``THE``, ``AND``, ``SHE``, ``HIS``, ``HER``, etc (etc not so).

At the end of the report the ``Sybil`` has shown to Eve that there is a strong possibility of a 3-letter
keyword usage.

Now, Eve started playing a ``Geek Hangman`` ;) For doing it she used another suggestive application mode called
``hangman``.

## The Hangman

She picked the first trigram reported (``GCI``) and made an assumption about it represent a ``THE`` when decrypted. So
translating it into commands would be:

```
eve@EDEN:~/src/tree/sin/curiosity# vigcrack --hangman --pattern=GCI\
> --plaintext=THE --key-len=3 --file-path=alice-bob-crap.txt --max-attempts=30
```

Now explaining the Eve's intentions. Eve is assuming ``GCI -> THE`` and also assuming that a keyword of length 3 was used.
The ``--file-path`` indicates the cryptogram's path and the option ``--max-attempts=30`` will only decrypt the first
30 letters from the file. This command spitted the following report to Eve:

```
*** The keyword was completely re-used 102 times before.
    The keyword's letter at position #1 was used to encipher it.
    The effective keyword to encrypt the pattern was 'NVE'.

*** Decryption attempts assuming that 'GCI' leads to 'THE'...

	K	C	P
	__________________
	N	Y	l
	V	L	q
	E	P	l
	N	H	u
	V	J	o
	E	V	r
	N	G	t
	V	I	n
	E	O	k
	N	V	i
	V	H	m
	E	W	s
	N	A	n
	V	D	i
	E	R	n
	N	G	t
	V	V	a
	E	I	e
	N	E	r
	V	H	m
	E	R	n
	N	J	w
	V	P	u
	E	T	p
	N	U	h
	V	C	h
	E	Y	u
	N	D	q
	V	S	x
	E	E	a
	__________________

```

The ``Hangman`` is interactive and you can get lots of fun when you start making right assumptions.

However, Eve have seen that her guessing did not produced a legible output. So, she returned to the pattern listing and
started trying other ones. After several attempts she have decided to use another application mode called ``brutus``.

## The Brutus

Well, in fact the ``brutus`` mode does what the ``hangman`` does but automated. Then, Eve have emitted the following
command into her terminal:

```
eve@EDEN:~/src/tree/sin/curiosity# vigcrack --brutus --file-path=alice-bob-crap.txt\
> --plaintexts=the,and,ing,ent,ion,nth,ter,int,oft,tha,ere,tio,her,fth,eth,ati,hat,ate,sth,est\
> --key-len=3 > report.txt
```

With this command ``brutus`` tries to decipher any found pattern to the informed plaintexts assuming the passed key length.

Eve got the following data into ``report.txt`` file:

```
(...)
```

Enjoy!
