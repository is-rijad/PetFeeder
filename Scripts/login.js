document.addEventListener("keypress", e => {
    if (e.key == "Enter")
        login();
});

function login() {
    let email = document.getElementById('EmailInput');
    let lozinka = document.getElementById('LozinkaInput');

    if (ValidirajEmail(email.value)
        && ValidirajLozinku(lozinka.value)) {
        UlogujKorisnika();
    }
    else {
        porukaError('Unešeni podaci nisu ispravni!');
        email.value = '';
        lozinka.value = '';
    }
}

function UlogujKorisnika() {
    let email = document.getElementById('EmailInput').value;
    let lozinka = document.getElementById('LozinkaInput').value;

    let url = `${urlAplikacije}/Korisnik/UlogujKorisnika`;

    let obj = new Object();
    obj.email = email;
    obj.lozinka = encoder(lozinka);

    fetch(url, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(obj)
    }).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku: " + response.statusText);
            return;
        }
        response.json().then(r => {
            sessionStorage.setItem(korisnikId, r.korisnikID);
            redirekcija("/");
        })
    }).catch(error => {
        porukaError("Greška u komunikaciji sa serverom " + error.statusText);
    })
}