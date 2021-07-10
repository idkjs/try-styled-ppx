
let baseUrl = "http://localhost:3003/";
let countersUrl = baseUrl ++ "counters";

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
let handleCounterData = data => {
  /* read the file from disk */
  // let file_content = Node.Fs.readFileAsUtf8Sync(event_file);
  /* parse the json */
  // logfake(data)->ignore;
  Js.log2("handleCounterData:", data);

  // let json = Js.Json.parseExn(data);
  let dict = Js.Dict.empty();
  Js.Dict.set(dict, "counters", data);
  let json: Js.Json.t = Js.Json.object_(dict);
  /* turn it into a proper record */
  let counters: Counter_t.counters =
    Atdgen_codec_runtime.Decode.decode(Counter_bs.read_counters, json);
  counters;
};
let fetchCounters = (): Js.Promise.t(option(Js.Json.t)) => {
  // let json = Js.Json.parseExn(url)|>Obj.magic|>promiseToOptionalJson;
  // let json = Js.Json.parseExn(url);
  Js.log("Called");

  let x = Fetch.fetch(countersUrl) |> promiseToOptionalJson;
  Js.log(x)->Js.Promise.resolve->ignore;
  Fetch.fetch(countersUrl) |> promiseToOptionalJson;
};
let authenticate = (res: Fetch.Response.t): Js.Promise.t(Js.Json.t) => {
  let status = Fetch.Response.status(res);
  switch (status) {
  | 200 => Fetch.Response.json(res)
  | 401 =>
    Js.log("/login");
    Js.Exn.raiseError(Fetch.Response.statusText(res));
  | _ => Js.Exn.raiseError(Fetch.Response.statusText(res))
  };
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
