using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using PetFeederWebApi.Data;
using PetFeederWebApi.Models;
using PetFeederWebApi.Models.ViewModels;

namespace PetFeederWebApi.Controllers {
    [ApiController]
    [Route("[controller]/[action]")]
    public class KorisnikUredjajController : ControllerBase
    {
        private readonly PetFeederContext _context;

        public KorisnikUredjajController(PetFeederContext context)
        {
            _context = context;
        }

        [HttpPost]
        public IActionResult DodajUredjajZaKorisnika([FromBody] KorisnikUredjajVM korisnikUredjaj) {
            if (_context.Korisnici.Count() == 0) {
                return BadRequest("Ne postoji nijedan korisnik");
            }
            _context.Uredjaji.Add(new Uredjaj()
            {
                Ime = korisnikUredjaj.uredjajIme,
                Mac = korisnikUredjaj.uredjajMac,
                Izbacivanja = 0
                });
            _context.SaveChanges();
            var uredjaj = _context.Uredjaji.OrderByDescending(id => id.UredjajID).FirstOrDefault();
            if (uredjaj == null) {
                return NotFound("Uredjaj ne postoji");
            }
            var korisnik = _context.Korisnici.Where(k => k.KorisnikID == korisnikUredjaj.korisnikID).FirstOrDefault();
            if (korisnik == null) {
                return NotFound("Korisnik ne postoji");
            }
            _context.KorisniciUredjaji.Add(new KorisnikUredjaj()
            {
                KorisnikID = korisnikUredjaj.korisnikID,
                UredjajID = uredjaj.UredjajID,
            });
            _context.SaveChanges();
            return Ok();
        }

        [HttpGet]
        public ActionResult GetUredjajeZaKorisnika([FromQuery] int korisnikId) {
            var uredjaji = _context.KorisniciUredjaji.Include(u => u.Uredjaj).Where(id => id.KorisnikID == korisnikId).Select(uredjaj => uredjaj.Uredjaj).ToList();
            if (uredjaji == null) {
                return BadRequest("Korisnik nema uredjaja!");
            }
            return Ok(uredjaji);
        }

        [HttpDelete]
        public IActionResult DeleteUredjaj([FromQuery] int korisnikId, [FromQuery] int uredjajId) {
            var uredjaj = _context.KorisniciUredjaji.Where(ku => ku.KorisnikID == korisnikId && ku.UredjajID == uredjajId).FirstOrDefault();
            if (uredjaj == null)
                return BadRequest("Uredjaj ne postoji");
            _context.Aktivacije.Where(a => a.UredjajID == uredjaj.UredjajID).ExecuteDelete();
            _context.KorisniciUredjaji.Where(a => a.UredjajID == uredjaj.UredjajID).ExecuteDelete();
            _context.Uredjaji.Where(a => a.UredjajID == uredjaj.UredjajID).ExecuteDelete();
            _context.SaveChanges();
            return Ok();
        }
    }
}
