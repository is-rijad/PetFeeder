let email = document.getElementById('EmailInput');

document.addEventListener("keypress", e => {
    if (e.key == "Enter")
        resetLozinke();
});

function resetLozinke() {
    if (ValidirajEmail(email.value)) {
        let url = `${urlAplikacije}/Korisnik/PosaljiEmail?email=${email.value}`;

        fetch(url).then(response => {
            if (response.status != 200) {
                response.text().then((value) => {
                    porukaError("Server javlja grešku: " + value);
                })
                return;
            }
            porukaSuccess("Lozinka je uspješno resetovana.\nProvjerite E-Mail inbox.");
            document.getElementById('EmailInput').value = '';
        }).catch(error => {
            porukaError("Greška u komunikaciji sa serverom, pokušajte ponovo!");
        })
    }
    else {
        porukaError('Unešeni E-Mail nije ispravan!');
        email.value = '';
    }
}