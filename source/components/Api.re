
let baseUrl = "http://127.0.0.1:3003/";
let countersUrl = baseUrl ++ "counters";
let endpoint = "http://127.0.0.1:3003/counters";

type t = RemoteData.t((Counters.Counter_t.counters));

exception DecodeError(string);
exception ServerError(string);

let raiseOnNok = (promise: Js.Promise.t(Fetch.Response.t)) => {
  promise
  |> Js.Promise.then_(r => {
       Js.log2("Called raiseOnNok", r);
       r |> Fetch.Response.ok
         ? promise : Js.Exn.raiseError(Fetch.Response.statusText(r));
     });
};
let promiseToOptionalJson =
    (promise: Js.Promise.t(Fetch.response))
    : Js.Promise.t(option(Js.Json.t)) => {
  promise
  |> raiseOnNok
  |> Js.Promise.then_(Fetch.Response.json)
  |> Js.Promise.then_(v => {
       let v = v->Some->Js.Promise.resolve;
       Js.log2("Called promiseToOptionalJson", v);
       v;
     })
  |> Js.Promise.catch(e => {
       Js.log2("Unexpected error: ", e);
       None->Js.Promise.resolve;
     });
};


 let fetchCounters = ()=> Js.Promise.(
    Fetch.fetch(countersUrl)
    |> then_(res => Fetch.Response.json(res))
    |> then_(json => json |> HandleCounterData.handleCounterData |> resolve)
  );
// let fetchCounters = () => {
//   Js.Promise.(
//     Fetch.fetch(countersUrl)
//     |> then_(Fetch.Response.json)
//     |> then_(json =>
//          switch (Counters.Counters_bs.read_t(json)) {
//          | Ok(res) => resolve(res)
//          | Error(err) =>
//            reject(DecodeError({j|Decode error in ($path): $message|j}))
//          }
//        )
//     |> catch(_ => reject(ServerError("Something went wrong")))
//   );
// };
