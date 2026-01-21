let email = document.getElementById('EmailInput');
let lozinka = document.getElementById('LozinkaInput');

document.addEventListener("keypress", e => {
    if (e.key == "Enter")
        registracija();
});

function registracija() {
    if (ValidirajEmail(email.value)
        && ValidirajLozinku(lozinka.value)) {
        registrujKorisnika();
    }
    else {
        porukaError('Unešeni podaci nisu ispravni!');
        porukaError('Lozinka mora sadržavati najmanje 8 znakova, velika i mala slova i brojeve.');
        email.value = '';
        lozinka.value = '';
    }
}

function registrujKorisnika() {
    let url = `${urlAplikacije}/Korisnik/RegistrujKorisnika`;

    let obj = new Object();
    obj.email = email.value;
    obj.lozinka = encoder(lozinka.value);

    fetch(url, {
        method: 'POST',
        headers: {
            'Content-type': 'application/json',
        },
        body: JSON.stringify(obj)
    }).then(r => {
        if (r.status != 200) {
            r.text().then((value) => {
                porukaError("Server javlja grešku: " + value);
            })
            return;
        }
        redirekcija('../login.html');
    }).catch(err => {
        porukaError("Greška u komunikaciji sa serverom, pokušajte ponovo!");
    })
}