using Microsoft.AspNetCore.Mvc;

namespace PetFeederWebApi.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class Status : ControllerBase
    {

        [HttpGet]
        public IActionResult GetStatus()
        {
            return Ok();
        }
    }
}