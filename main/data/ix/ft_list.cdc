import SwapFactory from 0xb063c16cac85dbd1
import StableSwapFactory from 0xb063c16cac85dbd1
import SwapInterfaces from 0xb78ef7afa52ff906
import NonFungibleToken from 0x1d7e57aa55817448
import FungibleToken from 0xf233dcee88fe0abe
import FungibleTokenMetadataViews from 0xf233dcee88fe0abe
import Icons from 0x5f195aa4322e5dbb
import MetadataViews from 0x1d7e57aa55817448

pub fun main(addr: Address):  AnyStruct {
        let acct = getAccount(addr)

      var ft : [AnyStruct] = []

      getAuthAccount(addr).forEachStored(fun (path: StoragePath, type: Type): Bool {

        if type.isSubtype(of: Type<@FungibleToken.Vault>()){
          var vault = getAuthAccount(addr).borrow<&FungibleToken.Vault>(from:path)!
          if (vault.balance==0.0) {
            return true
          }
		      var identifier = type.identifier.slice(from: 0, upTo:  type.identifier.length-6)

            var idf = type.identifier
            if idf.slice(from: idf.length-6, upTo:idf.length)==".Vault"{
                     idf = idf.slice(from: 0, upTo: idf.length-6)
            }

             var image = ""
               if let img = Icons.icons[idf]{
                 image = img
               }

          var name = type.identifier.slice(from: 19, upTo: type.identifier.length);
          if name.length>6{
            if name.slice(from: name.length-6, upTo:name.length)==".Vault"{
			        name = name.slice(from: 0, upTo: name.length-6)
            }
          }

		  var price : UFix64 = 0.0
			var swapInfo = SwapFactory.getPairInfo(token0Key: identifier, token1Key: "A.b19436aae4d94622.FiatToken") as? [AnyStruct]?
			if let si = swapInfo{
       var  p2 =  si![2] as! UFix64
       if p2>0.0{
				price =  si![3] as! UFix64 / si![2] as! UFix64
        }else{
        price=0.0
        }
			}
		if identifier=="A.b19436aae4d94622.FiatToken"{
			price = 1.0;
		}

		  var totalValue = price * vault.balance

          var displayInfo : {String:String} = {"image":image, "name":name, "identifier": name, "balance":vault.balance.toString(), "price": price.toString(), "total":totalValue.toString()}
          if let data = vault.resolveView(Type<FungibleTokenMetadataViews.FTDisplay>()){
            if let display = data as? FungibleTokenMetadataViews.FTDisplay{
                displayInfo["name"] = display.name
            }
          }
          ft.append(displayInfo)
        }
        return true
      })

        return ft
 }
